#include "hist_functions.h"

static std::vector<uint32_t> random_vec(size_t n) {
    std::mt19937 rng(SEED);

    std::vector<uint32_t> rngs;
    rngs.reserve(n);
    for (size_t i = 0; i != SAMPLES; ++i) {
        rngs.push_back(rng());
    }
    return rngs;
}

Histogram hist_find_uint32_t_raw(size_t n) {
    auto rngs = random_vec(n);

    Histogram                    stats;
    RawHashMap<uint32_t, size_t> map;
    for (size_t i = 0; i < n; i++) {
        map[rngs[i]] += 1;
    }
    for (size_t r = 0; r < SAMPLES; r++) {
        auto num = rngs[r % n];
        asm volatile("" ::: "memory");
        uint64_t t0 = rdtsc_start();

        auto* p = map.get(num);

        asm volatile("" ::: "memory");
        doNotOptimize(p);

        uint64_t t1 = rdtsc_end();
        sink += *p;
        stats.observe((double)(t1 - t0));
    }
    return stats;
}

Histogram hist_find_uint32_t_google(size_t n) {
    auto rngs = random_vec(n);

    Histogram                                stats;
    google::dense_hash_map<uint32_t, size_t> map;
    map.set_empty_key(std::numeric_limits<uint32_t>::max());
    for (size_t i = 0; i < n; i++) {
        map[rngs[i]] += 1;
    }
    for (size_t r = 0; r < SAMPLES; r++) {
        auto num = rngs[r % n];
        asm volatile("" ::: "memory");
        uint64_t t0 = rdtsc_start();

        auto p = map.find(num);

        asm volatile("" ::: "memory");
        doNotOptimize(p);

        uint64_t t1 = rdtsc_end();
        sink += p->second;
        stats.observe((double)(t1 - t0));
    }
    return stats;
}

Histogram hist_insert_uint32_t_google(size_t n) {
    auto      rngs = random_vec(n);
    Histogram stats;
    size_t    its = 0;

    while (its < SAMPLES) {
        google::dense_hash_map<uint32_t, size_t> map;
        map.set_empty_key(std::numeric_limits<uint32_t>::max());

        for (size_t r = 0; r < n && its < SAMPLES; r++, ++its) {
            auto num = rngs[r];

            asm volatile("" ::: "memory");
            uint64_t t0 = rdtsc_start();

            auto& p = map[num] += 1;

            asm volatile("" ::: "memory");
            doNotOptimize(p);

            uint64_t t1 = rdtsc_end();
            sink += p;
            stats.observe((double)(t1 - t0));
        }
    }
    return stats;
}

Histogram hist_insert_uint32_t_raw(size_t n) {
    auto      rngs = random_vec(n);
    Histogram stats;
    size_t    its = 0;

    while (its < SAMPLES) {
        RawHashMap<uint32_t, size_t> map;

        for (size_t i = 0; i < n && its < SAMPLES; i++, ++its) {
            auto num = rngs[its];

            asm volatile("" ::: "memory");
            uint64_t t0 = rdtsc_start();

            auto& p = map[num] += 1;

            asm volatile("" ::: "memory");
            doNotOptimize(p);

            uint64_t t1 = rdtsc_end();
            sink += p;
            stats.observe((double)(t1 - t0));
        }
    }
    return stats;
}

Histogram hist_find_string_view_raw(size_t n) {
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

        auto* p = map.get(word);

        asm volatile("" ::: "memory");
        doNotOptimize(p);

        uint64_t t1 = rdtsc_end();
        sink += *p;
        stats.observe((double)(t1 - t0));
    }
    return stats;
}

Histogram hist_find_string_view_google(size_t n) {
    Histogram stats;
    n = std::min(n, g_corpus.words.size());

    google::dense_hash_map<std::string_view, size_t> map;
    map.set_empty_key(EMPTY_SV);

    for (size_t i = 0; i < n; i++) {
        map[g_corpus.words[i]] += 1;
    }

    for (size_t its = 0; its != SAMPLES; ++its) {
        auto word = g_corpus.words[its % n];

        asm volatile("" ::: "memory");
        uint64_t t0 = rdtsc_start();

        auto p = map.find(word);

        asm volatile("" ::: "memory");

        doNotOptimize(p);

        uint64_t t1 = rdtsc_end();
        sink += p->second;
        stats.observe((double)(t1 - t0));
    }
    return stats;
}

Histogram hist_insert_string_view_google(size_t n) {
    Histogram stats;
    size_t    its = 0;

    n = std::min(n, g_corpus.words.size());

    while (its < SAMPLES) {
        google::dense_hash_map<std::string_view, size_t> map;
        map.set_empty_key(EMPTY_SV);

        for (size_t r = 0; r < n && its < SAMPLES; r++, ++its) {
            auto word = g_corpus.words[r];

            asm volatile("" ::: "memory");
            uint64_t t0 = rdtsc_start();

            auto p = map[word] += 1;

            asm volatile("" ::: "memory");
            doNotOptimize(p);

            uint64_t t1 = rdtsc_end();
            stats.observe((double)(t1 - t0));
        }
    }
    return stats;
}

Histogram hist_insert_string_view_raw(size_t n) {
    Histogram stats;
    size_t    its = 0;

    n = std::min(n, g_corpus.words.size());

    while (its < SAMPLES) {
        RawHashMap<std::string_view, size_t> map;
        for (size_t r = 0; r < n && its < SAMPLES; r++, ++its) {
            auto word = g_corpus.words[r];

            asm volatile("" ::: "memory");
            uint64_t t0 = rdtsc_start();

            auto p = map[word] += 1;

            asm volatile("" ::: "memory");
            doNotOptimize(p);

            uint64_t t1 = rdtsc_end();
            stats.observe((double)(t1 - t0));
        }
    }
    return stats;
}
