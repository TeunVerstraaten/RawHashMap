#ifndef __HIST_FUNCTIONS__H_
#define __HIST_FUNCTIONS__H_

#include "bench_tools.h"
#include "corpus.h"
#include "histogram.h"
#include "raw_hash_map.h"

#include <random>
#include <sparsehash/dense_hash_map>
#include <sparsehash/internal/densehashtable.h>
#include <string_view>

static constexpr size_t SAMPLES = 5'000'000;
static constexpr size_t SEED    = 12344321;
static volatile int     sink;
static const Corpus     g_corpus = load_corpus("text_2.txt");

static constexpr auto EMPTY_STR = "";
static constexpr auto EMPTY_SV  = std::string_view{EMPTY_STR};

inline Histogram hist_find_raw_uint32_t(size_t n) {
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

inline Histogram hist_find_google_uint32_t(size_t n) {
    std::mt19937 rng(SEED);
    Histogram    stats;
    size_t       its = 0;

    while (its < SAMPLES) {
        google::dense_hash_map<uint32_t, size_t> map;
        map.set_empty_key(std::numeric_limits<uint32_t>::max());
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

inline Histogram hist_add_uint32_t_google(size_t n) {
    Histogram stats;

    size_t       its = 0;
    std::mt19937 rng(SEED);

    while (its < SAMPLES) {
        google::dense_hash_map<uint32_t, size_t> map;
        map.set_empty_key(std::numeric_limits<uint32_t>::max());

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
template <typename Map>
inline Histogram hist_add_uint32_t(size_t n) {
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

inline Histogram hist_find_raw_string_view(size_t n) {
    Histogram stats;
    size_t    its = 0;

    RawHashMap<std::string_view, size_t> map;
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

inline Histogram hist_find_google_string_view(size_t n) {
    Histogram stats;
    size_t    its = 0;

    google::dense_hash_map<std::string_view, size_t> map;
    map.set_empty_key(EMPTY_SV);

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

inline Histogram hist_add_string_view_google(size_t n) {
    Histogram stats;
    size_t    its = 0;

    google::dense_hash_map<std::string_view, size_t> map;
    map.set_empty_key(EMPTY_SV);

    if (n > g_corpus.words.size()) {
        n = g_corpus.words.size();
    }
    for (size_t i = 0; i < n; i++) {}

    while (its < SAMPLES) {
        for (size_t r = 0; r < n && its < SAMPLES; r++) {
            ++its;

            asm volatile("" ::: "memory");
            uint64_t t0 = rdtsc_start();

            auto p = map[g_corpus.words[r]] += 1;

            asm volatile("" ::: "memory");
            doNotOptimize(p);

            uint64_t t1 = rdtsc_end();
            stats.observe((double)(t1 - t0));
        }
    }
    return stats;
}
template <typename Map>
inline Histogram hist_add_string_view(size_t n) {
    Histogram stats;
    size_t    its = 0;

    Map map;
    if (n > g_corpus.words.size()) {
        n = g_corpus.words.size();
    }
    for (size_t i = 0; i < n; i++) {}

    while (its < SAMPLES) {
        for (size_t r = 0; r < n && its < SAMPLES; r++) {
            ++its;

            asm volatile("" ::: "memory");
            uint64_t t0 = rdtsc_start();

            auto p = map[g_corpus.words[r]] += 1;

            asm volatile("" ::: "memory");
            doNotOptimize(p);

            uint64_t t1 = rdtsc_end();
            stats.observe((double)(t1 - t0));
        }
    }
    return stats;
}

#endif // !__HIST_FUNCTIONS__H_
