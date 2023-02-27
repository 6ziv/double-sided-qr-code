#pragma once
#include <array>
#include <vector>
#include <initializer_list>
std::vector<std::vector<int>> alignment_pos_table = {
			{6},
			{6,18},
			{6,22},
			{6,26},
			{6,30},
			{6,34},
			{6,22,38},
			{6,24,42},
			{6,26,46},
			{6,28,50},
			{6,30,54},
			{6,32,58},
			{6,34,62},
			{6,26,46,66},
			{6,26,48,70},
			{6,26,50,74},
			{6,30,54,78},
			{6,30,56,82},
			{6,30,58,86},
			{6,34,62,90},
			{6,28,50,72,94},
			{6,26,50,74,98},
			{6,30,54,78,102},
			{6,28,54,80,106},
			{6,32,58,84,110},
			{6,30,58,86,114},
			{6,34,62,90,118},
			{6,26,50,74,98,122},
			{6,30,54,78,102,126},
			{6,26,52,78,104,130},
			{6,30,56,82,108,134},
			{6,34,60,86,112,138},
			{6,30,58,86,114,142},
			{6,34,62,90,118,146},
			{6,30,54,78,102,126,150},
			{6,24,50,76,102,128,154},
			{6,28,54,80,106,132,158},
			{6,32,58,84,110,136,162},
			{6,26,54,82,110,138,166},
			{6,30,58,86,114,142,170}
};




struct block_info_t {
	const int cnt;
	const int db;
	const int ecb;
	block_info_t(int c, int d, int e) :cnt(c), db(d), ecb(e) {}
};
struct blocks_info_t {
	const int tolerance;
	const std::vector<block_info_t> block_types;
	blocks_info_t(int r, int c1, int d1, int e1) :tolerance(r), block_types({ block_info_t(c1,d1,e1) }) {}
	blocks_info_t(int r, int c1, int d1, int e1, int c2, int d2, int e2) :tolerance(r), block_types({ block_info_t(c1,d1,e1),block_info_t(c2,d2,e2) }) {}
};
using version_blk_info = std::array<block_info_t, 4>;
const std::array<blocks_info_t, 4> block_info[40] = {
	{blocks_info_t(2,1,19,7),blocks_info_t(4,1,16,10),blocks_info_t(6,1,13,13),blocks_info_t(8,1,9,17)},
	{blocks_info_t(4,1,34,10),blocks_info_t(8,1,28,16),blocks_info_t(11,1,22,22),blocks_info_t(14,1,16,28)},
	{blocks_info_t(7,1,55,15),blocks_info_t(13,1,44,26),blocks_info_t(9,2,17,18),blocks_info_t(11,2,13,22)},
	{blocks_info_t(10,1,80,20),blocks_info_t(9,2,32,18),blocks_info_t(13,2,24,26),blocks_info_t(8,4,9,16)},
	{blocks_info_t(13,1,108,26),blocks_info_t(12,2,43,24),blocks_info_t(9,2,15,18,2,16,18),blocks_info_t(11,2,11,22,2,12,22)},
	{blocks_info_t(9,2,68,18),blocks_info_t(8,4,27,16),blocks_info_t(12,4,19,24),blocks_info_t(14,4,15,28)},
	{blocks_info_t(10,2,78,20),blocks_info_t(9,4,31,18),blocks_info_t(9,2,14,18,4,15,18),blocks_info_t(13,4,13,26,1,14,26)},
	{blocks_info_t(12,2,97,24),blocks_info_t(11,2,38,22,2,39,22),blocks_info_t(11,4,18,22,2,19,22),blocks_info_t(13,4,14,26,2,15,26)},
	{blocks_info_t(15,2,116,30),blocks_info_t(11,3,36,22,2,37,22),blocks_info_t(10,4,16,20,4,17,20),blocks_info_t(12,4,12,24,4,13,24)},
	{blocks_info_t(9,2,68,18,2,69,18),blocks_info_t(13,4,43,26,1,44,26),blocks_info_t(12,6,19,24,2,20,24),blocks_info_t(14,6,15,28,2,16,28)},
	{blocks_info_t(10,4,81,20),blocks_info_t(15,1,50,30,4,51,30),blocks_info_t(14,4,22,28,4,23,28),blocks_info_t(12,3,12,24,8,13,24)},
	{blocks_info_t(12,2,92,24,2,93,24),blocks_info_t(11,6,36,22,2,37,22),blocks_info_t(13,4,20,26,6,21,26),blocks_info_t(14,7,14,28,4,15,28)},
	{blocks_info_t(13,4,107,26),blocks_info_t(11,8,37,22,1,38,22),blocks_info_t(12,8,20,24,4,21,24),blocks_info_t(11,12,11,22,4,12,22)},
	{blocks_info_t(15,3,115,30,1,116,30),blocks_info_t(12,4,40,24,5,41,24),blocks_info_t(10,11,16,20,5,17,20),blocks_info_t(12,11,12,24,5,13,24)},
	{blocks_info_t(11,5,87,22,1,88,22),blocks_info_t(12,5,41,24,5,42,24),blocks_info_t(15,5,24,30,7,25,30),blocks_info_t(12,11,12,24,7,13,24)},
	{blocks_info_t(12,5,98,24,1,99,24),blocks_info_t(14,7,45,28,3,46,28),blocks_info_t(12,15,19,24,2,20,24),blocks_info_t(15,3,15,30,13,16,30)},
	{blocks_info_t(14,1,107,28,5,108,28),blocks_info_t(14,10,46,28,1,47,28),blocks_info_t(14,1,22,28,15,23,28),blocks_info_t(14,2,14,28,17,15,28)},
	{blocks_info_t(15,5,120,30,1,121,30),blocks_info_t(13,9,43,26,4,44,26),blocks_info_t(14,17,22,28,1,23,28),blocks_info_t(14,2,14,28,19,15,28)},
	{blocks_info_t(14,3,113,28,4,114,28),blocks_info_t(13,3,44,26,11,45,26),blocks_info_t(13,17,21,26,4,22,26),blocks_info_t(13,9,13,26,16,14,26)},
	{blocks_info_t(14,3,107,28,5,108,28),blocks_info_t(13,3,41,26,13,42,26),blocks_info_t(15,15,24,30,5,25,30),blocks_info_t(14,15,15,28,10,16,28)},
	{blocks_info_t(14,4,116,28,4,117,28),blocks_info_t(13,17,42,26),blocks_info_t(14,17,22,28,6,23,28),blocks_info_t(15,19,16,30,6,17,30)},
	{blocks_info_t(14,2,111,28,7,112,28),blocks_info_t(14,17,46,28),blocks_info_t(15,7,24,30,16,25,30),blocks_info_t(12,34,13,24)},
	{blocks_info_t(15,4,121,30,5,122,30),blocks_info_t(14,4,47,28,14,48,28),blocks_info_t(15,11,24,30,14,25,30),blocks_info_t(15,16,15,30,14,16,30)},
	{blocks_info_t(15,6,117,30,4,118,30),blocks_info_t(14,6,45,28,14,46,28),blocks_info_t(15,11,24,30,16,25,30),blocks_info_t(15,30,16,30,2,17,30)},
	{blocks_info_t(13,8,106,26,4,107,26),blocks_info_t(14,8,47,28,13,48,28),blocks_info_t(15,7,24,30,22,25,30),blocks_info_t(15,22,15,30,13,16,30)},
	{blocks_info_t(14,10,114,28,2,115,28),blocks_info_t(14,19,46,28,4,47,28),blocks_info_t(14,28,22,28,6,23,28),blocks_info_t(15,33,16,30,4,17,30)},
	{blocks_info_t(15,8,122,30,4,123,30),blocks_info_t(14,22,45,28,3,46,28),blocks_info_t(15,8,23,30,26,24,30),blocks_info_t(15,12,15,30,28,16,30)},
	{blocks_info_t(15,3,117,30,10,118,30),blocks_info_t(14,3,45,28,23,46,28),blocks_info_t(15,4,24,30,31,25,30),blocks_info_t(15,11,15,30,31,16,30)},
	{blocks_info_t(15,7,116,30,7,117,30),blocks_info_t(14,21,45,28,7,46,28),blocks_info_t(15,1,23,30,37,24,30),blocks_info_t(15,19,15,30,26,16,30)},
	{blocks_info_t(15,5,115,30,10,116,30),blocks_info_t(14,19,47,28,10,48,28),blocks_info_t(15,15,24,30,25,25,30),blocks_info_t(15,23,15,30,25,16,30)},
	{blocks_info_t(15,13,115,30,3,116,30),blocks_info_t(14,2,46,28,29,47,28),blocks_info_t(15,42,24,30,1,25,30),blocks_info_t(15,23,15,30,28,16,30)},
	{blocks_info_t(15,17,115,30),blocks_info_t(14,10,46,28,23,47,28),blocks_info_t(15,10,24,30,35,25,30),blocks_info_t(15,19,15,30,35,16,30)},
	{blocks_info_t(15,17,115,30,1,116,30),blocks_info_t(14,14,46,28,21,47,28),blocks_info_t(15,29,24,30,19,25,30),blocks_info_t(15,11,15,30,46,16,30)},
	{blocks_info_t(15,13,115,30,6,116,30),blocks_info_t(14,14,46,28,23,47,28),blocks_info_t(15,44,24,30,7,25,30),blocks_info_t(15,59,16,30,1,17,30)},
	{blocks_info_t(15,12,121,30,7,122,30),blocks_info_t(14,12,47,28,26,48,28),blocks_info_t(15,39,24,30,14,25,30),blocks_info_t(15,22,15,30,41,16,30)},
	{blocks_info_t(15,6,121,30,14,122,30),blocks_info_t(14,6,47,28,34,48,28),blocks_info_t(15,46,24,30,10,25,30),blocks_info_t(15,2,15,30,64,16,30)},
	{blocks_info_t(15,17,122,30,4,123,30),blocks_info_t(14,29,46,28,14,47,28),blocks_info_t(15,49,24,30,10,25,30),blocks_info_t(15,24,15,30,46,16,30)},
	{blocks_info_t(15,4,122,30,18,123,30),blocks_info_t(14,13,46,28,32,47,28),blocks_info_t(15,48,24,30,14,25,30),blocks_info_t(15,42,15,30,32,16,30)},
	{blocks_info_t(15,20,117,30,4,118,30),blocks_info_t(14,40,47,28,7,48,28),blocks_info_t(15,43,24,30,22,25,30),blocks_info_t(15,10,15,30,67,16,30)},
	{blocks_info_t(15,19,118,30,6,119,30),blocks_info_t(14,18,47,28,31,48,28),blocks_info_t(15,34,24,30,34,25,30),blocks_info_t(15,20,15,30,61,16,30)},
};