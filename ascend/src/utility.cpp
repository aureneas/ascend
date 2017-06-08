#include <cstdlib>
#include <random>
#include "../include/utility.h"

signed char rsign() {
    return ((rand()%2 == 1) ? 1 : -1);
}

template <typename NUM>
NUM rand2(NUM n, u_16 d) {
    NUM total = 0;
    for (u_16 i = d; i > 0; --i)
        total += rand() % n;
    return total;
}

std::default_random_engine rgen;

template <>
double rand2<double>(double n, u_16 d) {
    double total = 0;
    std::uniform_real_distribution<double> dist(0.0, n);
    for (u_16 i = d; i > 0; --i)
        total += dist(rgen);
    return total;
}

template u_16 rand2<u_16>(u_16, u_16);
