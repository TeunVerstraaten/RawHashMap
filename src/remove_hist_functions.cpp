#include "bench_tools.h"
#include "corpus.h"
#include "hist_functions.h"
#include "raw_hash_map.h"

#include <random>
#include <sparsehash/dense_hash_map>

static constexpr size_t SAMPLES     = 5'000'000;
static constexpr size_t SEED        = 12345654321;
static const Corpus     g_corpus    = load_corpus("text.txt");
static constexpr auto   EMPTY_STR   = "__EMPTY__";
static constexpr auto   DELETED_STR = "__DELETED__";
static constexpr auto   EMPTY_SV    = std::string_view{EMPTY_STR};
static constexpr auto   DELETED_SV  = std::string_view{DELETED_STR};
static volatile int     sink;

static std::vector<uint32_t> get_random_vec(size_t n) {
    std::mt19937 rng(SEED);

    std::vector<uint32_t> rngs;
    rngs.reserve(n);
    for (size_t i = 0; i != SAMPLES; ++i) {
        rngs.push_back(rng());
    }
    return rngs;
}

Histogram hist_remove_uint32_t_raw(size_t n) {
    auto rngs = get_random_vec(n);

    Histogram                    stats;
    RawHashMap<uint32_t, size_t> map;
    for (size_t i = 0; i < n; i++) {
        map[rngs[i]] += 1;
    }
    for (size_t r = 0; r < SAMPLES; r++) {
        auto num = rngs[r % n];
        asm volatile("" ::: "memory");
        uint64_t t0 = rdtsc_start();

        auto v = map.remove(num);

        asm volatile("" ::: "memory");
        doNotOptimize(v);

        uint64_t t1 = rdtsc_end();
        sink += v;
        stats.observe((double)(t1 - t0));
    }
    return stats;
}

Histogram hist_remove_uint32_t_google(size_t n) {
    auto rngs = get_random_vec(n);

    Histogram                                stats;
    google::dense_hash_map<uint32_t, size_t> map;
    map.set_empty_key(std::numeric_limits<uint32_t>::max());
    map.set_deleted_key(std::numeric_limits<uint32_t>::max() - 1);

    for (size_t i = 0; i < n; i++) {
        map[rngs[i]] += 1;
    }
    for (size_t r = 0; r < SAMPLES; r++) {
        auto num = rngs[r % n];
        asm volatile("" ::: "memory");
        uint64_t t0 = rdtsc_start();

        auto p = map.erase(num);

        asm volatile("" ::: "memory");
        doNotOptimize(p);

        uint64_t t1 = rdtsc_end();
        sink += p;
        stats.observe((double)(t1 - t0));
    }
    return stats;
}

Histogram hist_remove_string_view_raw(size_t n) {
    Histogram stats;
    n = std::min(n, g_corpus.words.size());

    RawHashMap<std::string_view, size_t> map;
    for (size_t i = 0; i < n; i++) {
        map[g_corpus.words[i]] += 1;
    }
    for (size_t its = 0; its != SAMPLES; ++its) {
        auto word = g_corpus.words[its % n];

        asm volatile("" ::: "memory");
        uint64_t t0 = rdtsc_start();

        auto p = map.remove(word);

        asm volatile("" ::: "memory");
        doNotOptimize(p);

        uint64_t t1 = rdtsc_end();
        sink += p;
        stats.observe((double)(t1 - t0));
    }
    return stats;
}

Histogram hist_remove_string_view_google(size_t n) {
    Histogram stats;
    n = std::min(n, g_corpus.words.size());

    google::dense_hash_map<std::string_view, size_t> map;
    map.set_empty_key(EMPTY_SV);
    map.set_deleted_key(DELETED_SV);

    for (size_t i = 0; i < n; i++) {
        map[g_corpus.words[i]] += 1;
    }

    for (size_t its = 0; its != SAMPLES; ++its) {
        auto word = g_corpus.words[its % n];

        asm volatile("" ::: "memory");
        uint64_t t0 = rdtsc_start();

        auto p = map.erase(word);

        asm volatile("" ::: "memory");

        doNotOptimize(p);

        uint64_t t1 = rdtsc_end();
        sink += p;
        stats.observe((double)(t1 - t0));
    }
    return stats;
}
