//
// Created by user on 2024/9/12.
//

#include "sobol.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>

std::vector<unsigned int> generate_v32(unsigned int s, unsigned int a, const std::vector<unsigned int>& m) {
    std::vector<unsigned int> v;
    std::vector<unsigned int> _m {0};
    for (unsigned int k = 1; k <= 32; k++) {
        if (k <= s) {
            _m.push_back(m[k-1]);
            v.push_back(_m[k] << (32 - k));
        }
        else {
            unsigned int m_k = _m[k-s] ^ (_m[k-s] << s);
            if (s > 1) {
                for (unsigned int i = 1; i <= s-1; i++) {
                    m_k ^= ((a >> (s-1 - i)) & 1) * (_m[k-i] << i);
                }
            }
            _m.push_back(m_k);
            v.push_back(m_k << (32 - k));
        }
    }
    return v;
}

std::vector<JoeKuo> loadJoeKuo(const char *fp) {
    std::vector<JoeKuo> res;
    std::ifstream file(fp, std::ios::in);
    std::string line;
    std::getline(file, line);
    while (file.good()) {
        JoeKuo joeKuo;
        file >> joeKuo.d >> joeKuo.s >> joeKuo.a;
        for (unsigned int i = 0; i < joeKuo.s; i++) {
            unsigned int m;
            file >> m;
            joeKuo.m.push_back(m);
        }
        res.push_back(joeKuo);
    }
    file.close();
    return res;
}

std::vector<unsigned int> generate_v32(unsigned int d, const char *fp) {
    std::vector<unsigned int> v;
    if (d == 1) {
        for (unsigned int k = 1; k <= 32; k++) {
            v.push_back(1 << (32-k));
        }
        return v;
    }
    std::ifstream file(fp, std::ios::in);
    std::string line;
    for (unsigned int i = 0; i < d-1; i++) {
        std::getline(file, line);
    }
    unsigned int _d, s, a, _m;
    std::vector<unsigned int> m;
    file >> _d >> s >> a;
    for (unsigned int i = 0; i < s; i++) {
        file >> _m;
        m.push_back(_m);
    }
    file.close();
    return generate_v32(s, a, m);
}
