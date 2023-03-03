#pragma once
#include <vector>
#include <set>
#include <map>
#include <optional>
extern "C" {
#include <correct.h>
}

#include "reed_solomon_helper.hpp"
#include "qr_constants.hpp"
#include "crc.hpp"
#include "makeseg.hpp"
#include "save_matrix.hpp"

#include "logger.hpp"

typedef std::pair<int, int> coord;
typedef std::array<coord, 8> codeword;
std::vector<codeword> split_codewords(int version) {
	int code_size = version * 4 + 17;

	auto cmp = [](const std::pair<int, int>& ilhs, const std::pair<int, int>& irhs)->bool {
		auto lhs = ilhs; auto rhs = irhs;
		if (lhs.first > 6)lhs.first--;
		if (rhs.first > 6)rhs.first--;
		if (lhs.first / 2 != rhs.first / 2)return lhs.first > rhs.first;

		bool go_up = (lhs.first / 2) & 1;
		if (lhs.second != rhs.second) {
			bool t = lhs.second < rhs.second;
			return go_up ^ t;
		}

		return lhs.first > rhs.first;
	};
	std::set<std::pair<int, int>> avail_coords;
	for (int x = 0; x < code_size; x++) {
		for (int y = 0; y < code_size; y++) {
			avail_coords.emplace(x, y);
		}
	}
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			avail_coords.erase(std::pair<int, int>({ x,y }));
			avail_coords.erase(std::pair<int, int>({ code_size - 1 - x,y }));
			avail_coords.erase(std::pair<int, int>({ x,code_size - 1 - y }));
		}
	}

	auto alignment_positions = alignment_pos_table[version - 1];
	for (int x : alignment_positions) {
		for (int y : alignment_positions) {
			if (avail_coords.count({ x,y })) {
				for (int ix = x - 2; ix <= x + 2; ix++) {
					for (int iy = y - 2; iy <= y + 2; iy++) {
						avail_coords.erase({ ix,iy });
					}
				}
			}
		}
	}
	for (int i = 0; i < code_size; i++) {
		avail_coords.erase({ 6,i });
		avail_coords.erase({ i,6 });
	}
	for (int i = 0; i < 9; i++) {
		avail_coords.erase({ i,8 });
		avail_coords.erase({ 8,i });
	}
	for (int i = 0; i < 8; i++) {
		avail_coords.erase({ code_size - 1 - i,8 });
		avail_coords.erase({ 8,code_size - 1 - i });
	}
	if (version >= 7) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 6; j++) {
				avail_coords.erase({ code_size - 9 - i,j });
				avail_coords.erase({ j,code_size - 9 - i });
			}
		}
	}
	std::vector<std::pair<int, int>> vec_coords(avail_coords.begin(), avail_coords.end());
	std::sort(vec_coords.begin(), vec_coords.end(), cmp);

	std::vector<codeword> ret;
	ret.resize(vec_coords.size() / 8);
	for (int i = 0; i < ret.size(); i++) {
		for (int j = 0; j < 8; j++) {
			ret[i][j] = vec_coords[8 * i + j];
		}
	}
	return ret;
}

std::vector<std::pair<std::vector<codeword>, std::vector<codeword>>> interleave(int ver, int lv) {
	using tag = std::tuple<int, bool, int>;
	const auto& info = block_info[ver - 1][lv];

	std::vector<std::pair<std::vector<codeword>, std::vector<codeword>>> ret;
	int total_groups, total_data_codewords;
	if (info.block_types.size() == 1) {
		total_groups = info.block_types[0].cnt;
		total_data_codewords = info.block_types[0].db * info.block_types[0].cnt;
	}
	else {
		total_groups = info.block_types[0].cnt + info.block_types[1].cnt;
		total_data_codewords = info.block_types[0].db * info.block_types[0].cnt + info.block_types[1].db * info.block_types[1].cnt;
	}

	ret.resize(total_groups);

	auto codewords = split_codewords(ver);

	for (int group_id = 0; group_id < total_groups; group_id++) {
		int codeword_id = group_id;
		for (int j = 0; j < info.block_types[0].db; j++) {
			ret[group_id].first.push_back(codewords[codeword_id]);
			codeword_id += total_groups;
		}
		if (group_id >= info.block_types[0].cnt) {
			codeword_id -= info.block_types[0].cnt;
			ret[group_id].first.push_back(codewords[codeword_id]);
		}

		codeword_id = total_data_codewords + group_id;
		for (int j = 0; j < info.block_types[0].ecb; j++) {
			ret[group_id].second.push_back(codewords[codeword_id]);
			codeword_id += total_groups;
		}
	}
	return ret;
}

using fmt_crc = CRC<11, 0b10100110111>;
int get_mask_err(int lv, int msk) {
	const std::bitset<15> xor_mask = std::bitset<15>("010010000010101");
	auto fmt = fmt_crc::crc_cat<5>(8 * (lv ^ 1) + msk);

	for (size_t i = 0; i < 15; i++) {
		fmt[i] = fmt[i] ^ xor_mask[i];
	}
	int err = 0;
	if (fmt[7] != true)err++;
	for (int i = 0; i < 7; i++) {
		if (fmt[i] != fmt[14 - i])err++;
	}
	return err;
}
int get_best_mask(int lv) {
	int tmsk = -1;
	int minerr = 5;
	for (int msk : {0, 3, 5, 6, 7}) {
		int e = get_mask_err(lv, msk);
		if (e < minerr) {
			tmsk = msk;
			minerr = e;
		}
	}

	if (minerr < 5) {
		//printf("Using mask:%d\nError:%d\n", tmsk, minerr);
		return tmsk;
	}
	else return -1;
}

void prefill_position_bits(std::vector<std::vector<int>>& ret, int version) {
	int n = version * 4 + 17;

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			if (std::max({ x - 3,3 - x,y - 3,3 - y }) & 1) {
				ret[y][x] = 1;
			}
			else {
				ret[y][x] = 0;
			}
			if (x == 3 && y == 3)
				ret[y][x] = 1;
			ret[n - 1 - y][x] = ret[y][n - 1 - x] = ret[y][x];
		}
	}


	auto alignment_positions = alignment_pos_table[version - 1];
	for (int x : alignment_positions) {
		for (int y : alignment_positions) {
			if (x < 8 && y >= n - 8)continue;
			if (y < 8 && x >= n - 8)continue;
			if (x < 8 && y < 8)continue;

			for (int ix = x - 2; ix <= x + 2; ix++) {
				for (int iy = y - 2; iy <= y + 2; iy++) {
					if (std::max({ ix - x,x - ix,iy - y,y - iy }) & 1) {
						ret[iy][ix] = 0;
					}
					else {
						ret[iy][ix] = 1;
					}
				}
			}
		}
	}

	for (int i = 0; i < n; i++) {
		if (i & 1) {
			if (ret[6][i] == -1)ret[6][i] = 0;
			if (ret[i][6] == -1)ret[i][6] = 0;
		}
		else {
			if (ret[6][i] == -1)ret[6][i] = 1;
			if (ret[i][6] == -1)ret[i][6] = 1;
		}
	}
	return;
}
using ver_crc = CRC<13, 0b1111100100101>;

void prefill_format_info(std::vector<std::vector<int>>& ret, int version, int lv, int mask) {
	int n = version * 4 + 17;
	const std::bitset<15> xor_mask = std::bitset<15>("010010000010101");
	auto fmt = fmt_crc::crc_cat<5>(8 * (lv ^ 1) + mask);
	for (size_t i = 0; i < 15; i++) {
		fmt[i] = fmt[i] ^ xor_mask[i];
	}
	std::bitset<15> rev_fmt;
	for (size_t i = 0; i < 15; i++) {
		rev_fmt[i] = fmt[14 - i];
	}
	//for (int i = 0; i < 15; i++)printf("%d", fmt[i] ? 1 : 0); printf("\n");
	//for (int i = 0; i < 15; i++)printf("%d", rev_fmt[i] ? 1 : 0); printf("\n");
	for (int i = 0; i < 6; i++) {
		ret[i][8] = ret[8][i] = rev_fmt[i];
	}
	ret[8][7] = ret[7][8] = rev_fmt[6];
	ret[8][8] = rev_fmt[7];
	for (int i = 0; i < 7; i++) {
		ret[n - 1 - i][8] = ret[8][n - 1 - i] = rev_fmt[14 - i];
	}
	ret[n - 8][8] = ret[8][n - 8] = 1;

	//Version info
	auto r = ver_crc::crc_cat<6>(version);
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 3; j++) {
			ret[n - 11 + j][i] = ret[i][n - 11 + j] = (r[17 - (i * 3 + j)] ? 1 : 0);
		}
	}
	return;
}
std::vector<std::vector<int>> get_prefilled(int ver, int lv, int& mask) {
	mask = get_best_mask(lv);
	if (mask == -1)return std::vector<std::vector<int>>();
	//mask = 5;
	int n = ver * 4 + 17;
	std::vector<std::vector<int>> ret;
	ret.resize(n);
	for (auto& v : ret)v.resize(n, -1);

	prefill_position_bits(ret, ver);
	prefill_format_info(ret, ver, lv, mask);
	return ret;
}

coord null_coord = { -1,-1 };
std::map<coord, bool> solve_equations(std::vector<std::set<coord>> equations_) {
	auto equations = equations_;
	std::set<coord> variables;
	for (const auto& e : equations) {
		for (const auto& v : e) {
			if (v != null_coord)variables.insert(v);
		}
	}
	int m = (int)equations.size();
	int bad_equations = 0;
	std::map<coord, bool> ret;
	std::map<coord, int> tmp;
	std::vector<bool> used;
	used.resize(m);
	int progress = 0;
	for (auto var : variables) {
		log_progress_equations(progress++, (int)variables.size());
		
		tmp[var] = -1;
		for (int j = 0; j < m; j++) {
			if (!equations[j].contains(var))continue;
			if (used[j])continue;
			if (tmp[var] == -1) {
				tmp[var] = j;
				used[j] = true;
				continue;
			}
			for (auto elem : equations[tmp[var]]) {
				if (equations[j].contains(elem))equations[j].erase(elem); else equations[j].insert(elem);
			}
			if (equations[j].size() == 1 && equations[j].contains(null_coord))bad_equations++;
		}
	}
	log_progress_equations((int)variables.size(), (int)variables.size());
	for (auto iter = variables.crbegin(); iter != variables.crend(); iter++) {
		const auto& var = *iter;
		if (tmp[var] == -1)ret[var] = (rand() & 1);
		else {
			const auto& equation = equations[tmp[var]];
			bool b = false;
			for (auto elem : equation) {
				if (elem == var)continue;
				if (elem != null_coord && !ret.contains(elem)) [[unlikely]] {
					log_critical("Unexpected Error.");
					exit(1);
					//Really unexcepted situation: it should never happen.
				}
					if (elem == null_coord || ret[elem])
						b = !b;
			}
			ret[var] = b;
		}
	}
	return ret;
}

bool try_ver_lv(int ver, int lv, const std::string& str1, const std::string& str2, const char* save_path, int protect_factor = 1) {
	int n = ver * 4 + 17;
	int mask;
	auto prefilled = get_prefilled(ver, lv, mask);
	auto code_groups = interleave(ver, lv);
	auto mat1 = prefilled;
	auto mat2 = prefilled;

	auto seg1 = make_segment(str1, ver);
	auto seg2 = make_segment(str2, ver);
	std::vector<uint8_t> segchs1, segchs2;
	segchs1.resize(seg1.size() / 8);
	for (int i = 0; i < segchs1.size(); i++) {
		segchs1[i] = 0;
		for (int j = 0; j < 8; j++) {
			segchs1[i] = (segchs1[i] << 1) | seg1[8 * i + j];
		}
	}
	segchs2.resize(seg2.size() / 8);
	for (int i = 0; i < segchs2.size(); i++) {
		segchs2[i] = 0;
		for (int j = 0; j < 8; j++) {
			segchs2[i] = (segchs2[i] << 1) | seg2[8 * i + j];
		}
	}

	auto merged = prefilled;
	int err1 = (int)seg1.size() / 8;
	int err2 = (int)seg2.size() / 8;

	auto try_fill = [](decltype(prefilled)& mat, const codeword& codeword, bool mirror, const std::vector<bool>& seg, int offset, int err)->bool {
		bool fit = true;
		for (int j = 0; j < 8; j++) {
			auto cw = codeword[j];
			if (mirror)std::swap(cw.first, cw.second);
			if (mat[cw.second][cw.first] != -1)fit = false;
		}
		if (!fit) {
			return false;
		}

		for (int j = 0; j < 8; j++) {
			auto cw = codeword[j];
			if (mirror)std::swap(cw.first, cw.second);
			mat[cw.second][cw.first] = seg[offset + j];
		}
		return true;
	};
	int iter = 0;

	for (int g = 0; g < code_groups.size(); g++) {
		auto cg = code_groups[g];
		for (const auto& codeword : cg.first) {
			if (err1 > err2) {
				if (iter < seg1.size()) {
					bool b = try_fill(merged, codeword, false, seg1, iter, err1);
					try_fill(mat1, codeword, false, seg1, iter, err1);
					if (b)err1--;
				}
				if (iter < seg2.size()) {
					bool b = try_fill(merged, codeword, true, seg2, iter, err2);
					try_fill(mat2, codeword, true, seg2, iter, err2);
					if (b)err2--;
				}
			}
			else {
				if (iter < seg2.size()) {
					bool b = try_fill(merged, codeword, true, seg2, iter, err2);
					try_fill(mat2, codeword, true, seg2, iter, err2);
					if (b)err2--;
				}
				if (iter < seg1.size()) {
					bool b = try_fill(merged, codeword, false, seg1, iter, err1);
					try_fill(mat1, codeword, false, seg1, iter, err1);
					if (b)err1--;
				}
			}
			iter += 8;
		}
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (merged[i][j] == -1 && mat1[i][j] != -1)merged[i][j] = mat1[i][j];
			if (merged[i][j] == -1 && mat2[i][j] != -1)merged[i][j] = mat2[i][j];
		}
	}
	if (protect_factor * err1 >= block_info[ver - 1][lv].tolerance || protect_factor * err2 >= block_info[ver - 1][lv].tolerance) {
		return false;
	}
	err1 = err2 = 0;
	iter = 0;
	for (int g = 0; g < code_groups.size(); g++) {
		auto cg = code_groups[g];
		for (const auto& codeword : cg.first) {
			if (iter < seg1.size()) {
				bool match = true;
				for (int j = 0; j < 8; j++) {
					const auto& pt = codeword[j];
					if (mat1[pt.second][pt.first] != merged[pt.second][pt.first]) {
						match = false;
					}
				}
				if (!match)err1++;
			}
			if (iter < seg2.size()) {
				bool match = true;
				for (int j = 0; j < 8; j++) {
					const auto& pt = codeword[j];
					if (mat2[pt.second][pt.first] != merged[pt.second][pt.first]) {
						match = false;
					}
				}
				if (!match)err2++;
			}
			iter += 8;
		}
	}
	

	iter = 0;

	std::vector<std::set<coord>> equations;
	for (int g = 0; g < code_groups.size(); g++) {
		auto cg = code_groups[g];
		auto table = RS::generate_table(RS::generate_poly((int)cg.second.size()), (int)cg.first.size());
		for (int c_id = 0; c_id < cg.second.size(); c_id++) {
			for (int j = 0; j < 8; j++) {
				bool b1 = false;
				bool b2 = false;
				std::set<coord> equation1;
				std::set<coord> equation2;
				for (int d_id = 0; d_id < cg.first.size(); d_id++) {
					for (int k = 0; k < 8; k++) {
						if (table[cg.second.size() * 8 - 1 - (c_id * 8 + j)][cg.first.size() * 8 - 1 - (d_id * 8 + k)]) {
							auto c = cg.first[d_id][k];
							if (mat1[c.second][c.first] != -1) {
								b1 = b1 ^ (bool)mat1[c.second][c.first];
							}
							else if (mat2[c.second][c.first] != -1) {
								b1 = b1 ^ (bool)mat2[c.second][c.first];
							}
							else {
								equation1.insert(c);
							}
							coord mc;
							mc.second = c.first; mc.first = c.second;
							if (mat2[mc.second][mc.first] != -1) {
								b2 = b2 ^ (bool)mat2[mc.second][mc.first];
							}
							else if (mat1[mc.second][mc.first] != -1) {
								b2 = b2 ^ (bool)mat1[mc.second][mc.first];
							}
							else {
								equation2.insert(mc);
							}
						}
					}
				}
				auto c = cg.second[c_id][j];
				if (mat1[c.second][c.first] != -1) {
					b1 = b1 ^ (bool)mat1[c.second][c.first];
				}
				else if (mat2[c.second][c.first] != -1) {
					b1 = b1 ^ (bool)mat2[c.second][c.first];
				}
				else {
					equation1.insert(c);
				}
				coord mc;
				mc.second = c.first; mc.first = c.second;
				if (mat2[mc.second][mc.first] != -1) {
					b2 = b2 ^ (bool)mat2[mc.second][mc.first];
				}
				else if (mat1[mc.second][mc.first] != -1) {
					b2 = b2 ^ (bool)mat1[mc.second][mc.first];
				}
				else {
					equation2.insert(mc);
				}
				if (b1)equation1.insert(null_coord);
				if (b2)equation2.insert(null_coord);
				equations.push_back(equation1);
				equations.push_back(equation2);
			}
		}
	}
	auto solved = solve_equations(equations);
	for (const auto& solved_elem : solved) {
		int y = solved_elem.first.second;
		int x = solved_elem.first.first;
		merged[y][x] = solved_elem.second;
	}

	//Check.
	int iter_check = 0;
	for (int g = 0; g < code_groups.size(); g++) {
		auto cg = code_groups[g];
		std::vector<uint8_t> rs_encoded_block1, rs_encoded_block2;
		for (const auto& cw : cg.first) {
			uint8_t tch1 = 0;
			uint8_t tch2 = 0;
			for (int j = 0; j < 8; j++) {
				const auto& pt = cw[j];
				tch1 = (tch1 << 1) | merged[pt.second][pt.first];
				tch2 = (tch2 << 1) | merged[pt.first][pt.second];
			}
			rs_encoded_block1.push_back(tch1); rs_encoded_block2.push_back(tch2);
		}
		for (const auto& cw : cg.second) {
			uint8_t tch1 = 0;
			uint8_t tch2 = 0;
			for (int j = 0; j < 8; j++) {
				const auto& pt = cw[j];
				tch1 = (tch1 << 1) | merged[pt.second][pt.first];
				tch2 = (tch2 << 1) | merged[pt.first][pt.second];
			}
			rs_encoded_block1.push_back(tch1); rs_encoded_block2.push_back(tch2);
		}
		std::vector<uint8_t> decoded1, decoded2;
		decoded1.resize(cg.first.size()); decoded2.resize(cg.first.size());


		bool failing = false;
		correct_reed_solomon* rs = correct_reed_solomon_create(correct_rs_primitive_polynomial_8_4_3_2_0, 0, 1, cg.second.size());
		ssize_t k1 = correct_reed_solomon_decode(rs, rs_encoded_block1.data(), cg.first.size() + cg.second.size(), decoded1.data());
		ssize_t k2 = correct_reed_solomon_decode(rs, rs_encoded_block2.data(), cg.first.size() + cg.second.size(), decoded2.data());
		if (k1 < 0 || k2 < 0)failing = true;

		if (!failing) {
			for (int i = 0; i < cg.first.size(); i++) {
				if (iter_check < segchs1.size()) {
					if (decoded1[i] != segchs1[iter_check])failing = true;
				}
				if (iter_check < segchs2.size()) {
					if (decoded2[i] != segchs2[iter_check])failing = true;
				}
				iter_check++;
				if (failing)break;
			}
			if (!failing) {
				int e1 = 0; int e2 = 0;
				std::vector<uint8_t> re_encoded1, re_encoded2;
				re_encoded1.resize(cg.first.size() + cg.second.size());
				re_encoded2.resize(cg.first.size() + cg.second.size());
				correct_reed_solomon_encode(rs, decoded1.data(), cg.first.size(), re_encoded1.data());
				correct_reed_solomon_encode(rs, decoded2.data(), cg.first.size(), re_encoded2.data());
				for (int i = 0; i < cg.first.size() + cg.second.size(); i++) {
					if (re_encoded1[i] != rs_encoded_block1[i])e1++;
					if (re_encoded2[i] != rs_encoded_block2[i])e2++;
				}
				if (protect_factor * e1 >= block_info[ver - 1][lv].tolerance || protect_factor * e2 >= block_info[ver - 1][lv].tolerance)failing = true;

				//std::cout << "Group " << g << " Errors:" << e1 << " " << e2 << std::endl;
			}
		}
		correct_reed_solomon_destroy(rs);

		if (failing) {
			return false;
		}
	}

	//Masking
	std::function<bool(int, int)> masking_functions[8] = {
		[](int x,int y)->bool {return (x + y) % 2 == 0; },
		[](int x,int y)->bool {return y % 2 == 0; },
		[](int x,int y)->bool {return x % 3 == 0; },
		[](int x,int y)->bool {return (x + y) % 3 == 0; },
		[](int x,int y)->bool {return (x % 2 + y % 3) % 2 == 0; },
		[](int x,int y)->bool {return (x * y % 2 + x * y % 3) == 0; },
		[](int x,int y)->bool {return (x * y % 2 + x * y % 3) % 2 == 0; },
		[](int x,int y)->bool {return (x * y % 3 + (x + y) % 2) % 2 == 0; }
	};
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (prefilled[i][j] == -1) {
				if (merged[i][j] != -1)
					merged[i][j] ^= masking_functions[mask](j, i) ? 1 : 0;
			}
		}
	}
	save(merged, save_path);
	return true;
}

bool search_answer_p(const char* str1, const char* str2,const char* target, int protect_factor) {
	const char lvs[4] = { 'L','M','Q','H' };
	for (int ver = 1; ver <= 40; ver++) {
		for (int lv = 0; lv < 4; lv++) {
			//for (int lv = 0; lv < 1; lv++) { // only try Level L
			
			log_progress_ver_lv(ver, lv);
			if (try_ver_lv(ver, lv, str1, str2, target, protect_factor)) {
				return true;
			}
		}
	}
	return false;
}
bool search_answer(const char* str1, const char* str2, const char* target) {
	if (search_answer_p(str1, str2, target, 2))return true;
	log_critical("Cannot find answer. About to retry without protection margins.");
	if (search_answer_p(str1, str2, target, 1))return true;
	log_critical("Cannot find answer.");
	return false;
}