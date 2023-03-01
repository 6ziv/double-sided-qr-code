#pragma once
#include <string>
#include <vector>
std::vector<bool> make_8bit_seg(const std::string& str, int ver) {
	std::vector<bool> b = { 0,1,0,0 };
	int len = (int)str.length();
	int K;
	if (ver <= 9)K = 8;
	else K = 16;
	for (int i = 0; i < K; i++) {
		b.push_back(len & (1 << (K - 1)));
		len <<= 1;
	}

	int pos = 0;
	for (auto ch : str) {
		for (int i = 7; i >= 0; i--) {
			b.push_back(ch & (1 << i));
		}
	}
	for (int i = 0; i < 4; i++)b.push_back(false);
	while (b.size() % 8 != 0)b.push_back(false);
	return b;
}
std::vector<bool> make_alnum_seg(const std::string& str, int ver,bool& ok) {
	ok = true;
	std::vector<bool> b = { 0,0,1,0 };
	int len = (int)str.length();
	int K;
	if (ver <= 9)K = 9;
	else if (ver <= 26)K = 11;
	else K = 13;
	
	for (int i = 0; i < K; i++) {
		b.push_back(len & (1 << (K - 1)));
		len <<= 1;
	}

	int pos = 0;
	auto char_lookup = [](char ch)->int {
		if (ch >= '0' && ch <= '9')return ch - '0';
		if (ch >= 'A' && ch <= 'Z')return ch - 'A' + 10;
		//if (ch >= 'a' && ch <= 'z')return ch - 'a' + 10;
		const char table[10] = " $%*+-./:";
		for (int i = 0; i < 9; i++) {
			if (table[i] == ch)return 36 + i;
		}
		return -1;
	};
	while (pos < str.length()) {
		if (str.length() - pos == 1) {
			int ch = char_lookup(str[pos]);
			if (ch == -1) {
				ok = false; return {};
			}
			for (int i = 0; i < 6; i++) {
				b.push_back(ch & (1 << 5));
				ch <<= 1;
			}
			pos += 1;
		}
		else {
			int ch1 = char_lookup(str[pos]);
			int ch2 = char_lookup(str[pos + 1]);
			if (ch1 == -1 || ch2 == -1) {
				ok = false; return {};
			}
			int ch = ch1 * 45 + ch2;
			for (int i = 0; i < 11; i++) {
				b.push_back(ch & (1 << 10));
				ch <<= 1;
			}

			pos += 2;
		}
	}
	for (int i = 0; i < 4; i++)b.push_back(false);
	while (b.size() % 8 != 0)b.push_back(false);
	return b;
}
std::vector<bool> make_segment(const std::string& str,int ver) {
	std::vector<bool> v1 = make_8bit_seg(str, ver);
	bool is_alnum = true;
	auto v2 = make_alnum_seg(str, ver, is_alnum);
	if (is_alnum) {
		if (v2.size() < v1.size())return v2; else return v1;
	}
	else {
		return v1;
	}
}