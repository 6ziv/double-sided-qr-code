#pragma once
#include <cstdint>
#include <vector>
#include <algorithm>
#include <bitset>
namespace RS {
    uint8_t gflog[256];
    uint8_t gfexp[256];
    void RSinit() {
        gfexp[0] = 1;
        for (int i = 1; i < 255; i++) {
            int t = ((int)gfexp[i - 1]) << 1;
            if (t >= 256) t = t ^ 0b100011101;
            gfexp[i] = t;
        }
        for (int i = 0; i < 255; i++) {
            gflog[gfexp[i]] = i;
        }
    }
    uint8_t mul(uint8_t lhs, uint8_t rhs) {
        if (lhs == 0 || rhs == 0)return 0;
        return gfexp[(gflog[lhs] + gflog[rhs]) % 255];
    }
    std::vector<uint8_t> mul_poly(const std::vector<uint8_t>& lhs, const std::vector<uint8_t>& rhs) {
        std::vector<uint8_t> ret;
        ret.resize(lhs.size() + rhs.size() - 1);

        for (int i = 0; i < ret.size(); i++) {
            ret[i] = 0;
            for (int j = std::max<int>(0, i + 1 - (int)rhs.size()); j < std::min<int>((int)lhs.size(), i + 1); j++) {
                ret[i] ^= mul(lhs[j], rhs[i - j]);
            }
        }
        return ret;
    }
    std::vector<uint8_t> generate_poly(int n) {
        uint8_t t = 1;
        std::vector<uint8_t> ret = { 1 };
        for (int i = 0; i < n; i++) {
            ret = mul_poly(ret, { t,1 });
            t = mul(t, 2);
        }
        return ret;
    }
    std::vector<uint8_t> mod_poly(std::vector<uint8_t> x, const std::vector<uint8_t>& M) {
        while (x.size() >= M.size()) {
            size_t offset = x.size() - M.size();
            int K = x.back();
            for (int i = 0; i < M.size(); i++) {
                x[i + offset] ^= mul(M[i], K);
            }
            while (!x.empty() && x.back() == 0)x.pop_back();
        }
        if (x.empty())x.push_back(0);
        return x;
    }
    std::vector<std::vector<bool>> generate_table(const std::vector<uint8_t>& poly, int msg_len) {
        std::vector<std::vector<bool>> ret;
        ret.resize(poly.size() * 8 - 8);

        for (auto& v : ret)v.resize(msg_len * 8, false);


        for (int i = 0; i < msg_len; i++) {
            std::vector<uint8_t> tmp;
            tmp.resize(poly.size() + i, 0);
            tmp.back() = 1;
            for (int j = 0; j < 8; j++) {
                int id1 = 8 * i + j;
                auto tret = mod_poly(tmp, poly);
                tret.resize(poly.size() - 1, 0);
                for (int k = 0; k < poly.size() - 1; k++) {
                    for (int l = 0; l < 8; l++) {
                        int id2 = 8 * k + l;
                        if (tret[k] & (1 << l)) {
                            ret[id2][id1] = true;
                        }
                    }
                }
                tmp.back() = mul(tmp.back(), 2);
            }
        }
        return ret;
    }
};
