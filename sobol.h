//
// Created by user on 2024/9/12.
//

#ifndef SOBOL_H
#define SOBOL_H

#include <vector>

extern std::vector<unsigned int> generate_v32(unsigned int s, unsigned int a, const std::vector<unsigned int>& m);

typedef struct {
    unsigned int d;
    unsigned int s;
    unsigned int a;
    std::vector<unsigned int> m;
} JoeKuo;

extern std::vector<JoeKuo> loadJoeKuo(const char *fp);
extern std::vector<unsigned int> generate_v32(unsigned int d, const char *fp);

#endif //SOBOL_H
