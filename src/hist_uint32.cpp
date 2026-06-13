#include "bench_tools.h"
#include "histogram.h"
#include "raw_hash_map.h"

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <unordered_map>

static constexpr size_t SAMPLES = 5'000'000;
static constexpr size_t SEED    = 12344321;
volatile int            sink;

Histogram find_raw(size_t n) {
    std::mt19937 rng(SEED);
    Histogram    stats;
    size_t       its = 0;

    while (its < SAMPLES) {
        RawHashMap<uint32_t, size_t> map;
        {
            for (size_t i = 0; i < n; i++) {
                map[rng()] += 1;
            }
        }
        for (size_t r = 0; r < n && its < SAMPLES; r++) {
            ++its;

            asm volatile("" ::: "memory");
            uint64_t t0 = rdtsc_start();

            auto* p = map.get(rng());

            asm volatile("" ::: "memory");
            doNotOptimize(p);
            if (p) {
                size_t v = *p;
                sink += v;
            }
            uint64_t t1 = rdtsc_end();
            stats.observe((double)(t1 - t0));
        }
    }
    return stats;
}

Histogram find_unordered_map(size_t n) {
    std::mt19937 rng(SEED);
    Histogram    stats;
    size_t       its = 0;

    while (its < SAMPLES) {
        std::unordered_map<uint32_t, size_t> map;
        {
            for (size_t i = 0; i < n; i++) {
                map[rng()] += 1;
            }
        }
        for (size_t r = 0; r < n && its < SAMPLES; r++) {
            ++its;

            asm volatile("" ::: "memory");
            uint64_t t0 = rdtsc_start();

            auto p = map.find(rng());

            asm volatile("" ::: "memory");
            doNotOptimize(p);
            if (p != map.end()) {
                size_t v = p->second;
                sink += v;
            }
            uint64_t t1 = rdtsc_end();
            stats.observe((double)(t1 - t0));
        }
    }
    return stats;
}

template <typename Map>
Histogram add(size_t n) {
    Histogram stats;

    size_t       its = 0;
    std::mt19937 rng(SEED);

    while (its < SAMPLES) {
        Map map;

        for (size_t i = 0; i < n && its < SAMPLES; i++) {
            ++its;
            asm volatile("" ::: "memory");
            uint64_t t0 = rdtsc_start();

            auto& p = map[rng()] += 1;

            asm volatile("" ::: "memory");
            doNotOptimize(p);

            size_t v = p;
            sink += v;

            uint64_t t1 = rdtsc_end();
            stats.observe((double)(t1 - t0));
        }
    }
    return stats;
}

int main() {
    size_t min = 10;
    size_t max = 1'000'000;

    {
        std::ofstream outfile("find.dat");
        assert(outfile.is_open());

        outfile << "Cycles for finding element\n\n";
        for (size_t n = min; n <= max; n *= 10) {
            auto raw   = find_raw(n);
            auto unord = find_unordered_map(n);

            outfile << "n_elements: " << n << "\n";
            outfile << "n raw unordered\n";
            for (size_t i = 0; i != Histogram::N; ++i) {
                outfile << Histogram::BUCKETS[i] << " " << raw.counts()[i] << " " << unord.counts()[i] << "\n";
            }
            if (n != max) {
                outfile << "\n";
            }
        }
    }
    {
        std::ofstream outfile("add.dat");
        assert(outfile.is_open());

        outfile << "Cycles for adding element\n\n";
        for (size_t n = min; n <= max; n *= 10) {
            auto raw   = add<RawHashMap<uint32_t, size_t>>(n);
            auto unord = add<std::unordered_map<uint32_t, size_t>>(n);

            outfile << "n_elements: " << n << "\n";
            outfile << "n raw unordered\n";
            for (size_t i = 0; i != Histogram::N; ++i) {
                outfile << Histogram::BUCKETS[i] << " " << raw.counts()[i] << " " << unord.counts()[i] << "\n";
            }
            if (n != max) {
                outfile << "\n";
            }
        }
    }
}
