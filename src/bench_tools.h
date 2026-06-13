#ifndef __BENCH_TOOLS__H_
#define __BENCH_TOOLS__H_

#include <cctype>
#include <cmath>
#include <stdint.h>
#include <x86intrin.h>

static inline uint64_t rdtsc_start() {
    _mm_lfence();
    return __rdtsc();
}

static inline uint64_t rdtsc_end() {
    unsigned aux;
    uint64_t t = __rdtscp(&aux);
    _mm_lfence();
    return t;
}

template <class T>
inline void doNotOptimize(T const& value) {
    asm volatile("" : : "r,m"(value) : "memory");
}

#endif // !__BENCH_TOOLS__H_
