#pragma once
#include <cstdint>
#include <bitset>
template<size_t N, uint64_t P>
class CRC {
	using state_type = std::bitset<N - 1>;
	inline static std::bitset<N> Polynomial = std::bitset<N>(P);
	inline static void update_state(state_type& s, bool d) {
		bool t = s[0] ^ d;
		for (size_t i = 0; i < N - 2; i++) {
			s[i] = s[i + 1] ^ (Polynomial[N - 2 - i] & t);
		}
		s[N - 2] = t;
	}
	template<size_t M>
	inline static std::bitset<M> reverse(const std::bitset<M>& x) {
		std::bitset<M> ret;
		for (int i = 0; i < M; i++)
			ret[i] = x[M - 1 - i];
		return ret;
	}
public:
	template<size_t M>
	inline static state_type crc(const std::bitset<M>& x) {
		state_type st;
		for (size_t i = 0; i < M; i++)
			update_state(st, x[i]);
		return st;
	}
	
	template<size_t M>
	inline static state_type crc(uint64_t x) {
		return crc(reverse(std::bitset<M>(x)));
	}
	template<size_t M>
	inline static std::bitset<N - 1 + M> crc_cat(uint64_t x) {
		auto c = crc<M>(x);
		auto d = reverse(std::bitset<M>(x));
		std::bitset<N - 1 + M> r;
		for (size_t i = 0; i < M; i++)r[i] = d[i];
		for (size_t i = 0; i < N - 1; i++)r[i + M] = c[i];
		return r;
	}
};
