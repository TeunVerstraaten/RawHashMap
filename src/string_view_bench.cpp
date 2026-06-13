#include "bench_tools.h"
#include "corpus.h"
#include "histogram.h"
#include "raw_hash_map.h"

#include <cstdlib>
#include <iostream>
#include <string_view>
#include <unordered_map>

static const Corpus     g_corpus = load_corpus("text_2.txt");
static constexpr size_t SAMPLES  = 10'000'000;
volatile int            sink;

Histogram find_raw(size_t n) {
    Histogram stats;
    size_t    its = 0;

    RawHash<std::string_view, size_t> map;
    if (n > g_corpus.words.size()) {
        n = g_corpus.words.size();
    }
    for (size_t i = 0; i < n; i++) {
        map[g_corpus.words[i]] += 1;
    }
    while (its < SAMPLES) {
        for (size_t r = 0; r < n && its < SAMPLES; r++) {
            ++its;

            asm volatile("" ::: "memory");
            uint64_t t0 = rdtsc_start();

            auto* p = map.get(g_corpus.words[r]);

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
    Histogram stats;
    size_t    its = 0;

    std::unordered_map<std::string_view, size_t> map;
    if (n > g_corpus.words.size()) {
        n = g_corpus.words.size();
    }
    for (size_t i = 0; i < n; i++) {
        map[g_corpus.words[i]] += 1;
    }

    while (its < SAMPLES) {
        for (size_t r = 0; r < n && its < SAMPLES; r++) {
            ++its;

            asm volatile("" ::: "memory");
            uint64_t t0 = rdtsc_start();

            auto p = map.find(g_corpus.words[r]);

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

// template <typename Map>
// Histogram add(size_t n) {
//     Histogram stats;
//
//     size_t       its = 0;
//     std::mt19937 rng(SEED);
//     while (its < SAMPLES) {
//         Map map;
//
//         for (size_t i = 0; i < n && its < SAMPLES; i++) {
//             ++its;
//             asm volatile("" ::: "memory");
//             uint64_t t0 = rdtsc_start();
//
//             auto& p = map[rng()] += 1;
//
//             asm volatile("" ::: "memory");
//             doNotOptimize(p);
//
//             size_t v = p;
//             sink += v;
//
//             uint64_t t1 = rdtsc_end();
//             stats.observe((double)(t1 - t0));
//         }
//     }
//     return stats;
// }

int main() {
    size_t min = 10;
    size_t max = 1'000'000;
    for (size_t n = min; n <= max; n *= 10) {
        auto raw   = find_raw(n);
        auto unord = find_unordered_map(n);

        std::cout << "# find for n:" << n << "\n";
        for (size_t i = 0; i != Histogram::N; ++i) {
            std::cout << Histogram::BUCKETS[i] << " " << raw.counts()[i] << " " << unord.counts()[i] << "\n";
        }
        if (n != max) {
            std::cout << "\n";
            std::cout << "\n";
        }
    }
}
