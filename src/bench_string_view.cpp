#include "corpus.h"
#include "raw_hash_map.h"

#include <benchmark/benchmark.h>
#include <cctype>
#include <string>
#include <string_view>
#include <vector>

static const Corpus g_corpus = load_corpus("text_2.txt");

template <typename Map>
static void BM_add(benchmark::State& state) {
    size_t n = static_cast<size_t>(state.range(0));
    if (n > g_corpus.words.size()) {
        n = g_corpus.words.size();
    }

    for (auto _ : state) {
        Map map;
        // map.reserve(n);

        state.ResumeTiming();
        for (size_t i = 0; i < n; i++) {
            map[g_corpus.words[i]] += 1;
        }
        state.PauseTiming();

        benchmark::DoNotOptimize(map);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * n);
}

static void BM_add_unordered_map(benchmark::State& state) {
    BM_add<std::unordered_map<std::string_view, size_t>>(state);
}
static void BM_add_raw_hash_map(benchmark::State& state) {
    BM_add<RawHashMap<std::string_view, size_t>>(state);
}

static void BM_find_unordered_map(benchmark::State& state) {
    size_t n = static_cast<size_t>(state.range(0));
    if (n > g_corpus.words.size()) {
        n = g_corpus.words.size();
    }

    std::unordered_map<std::string_view, size_t> map;
    map.reserve(n);

    for (size_t i = 0; i < n; i++) {
        map[g_corpus.words[i]] += 1;
    }

    for (auto _ : state) {
        size_t c = 0;

        state.ResumeTiming();
        for (size_t i = 0; i < n; i++) {
            c += map.at(g_corpus.words[i]);
        }
        state.PauseTiming();

        benchmark::DoNotOptimize(c);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * n);
}

static void BM_find_raw_hash_map(benchmark::State& state) {
    size_t n = static_cast<size_t>(state.range(0));
    if (n > g_corpus.words.size()) {
        n = g_corpus.words.size();
    }

    RawHashMap<std::string_view, size_t> map;

    for (size_t i = 0; i < n; i++) {
        map[g_corpus.words[i]] += 1;
    }

    for (auto _ : state) {
        size_t c = 0;

        state.ResumeTiming();
        for (size_t i = 0; i < n; i++) {
            c += *map.get(g_corpus.words[i]);
        }
        state.PauseTiming();

        benchmark::DoNotOptimize(c);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * n);
}

BENCHMARK(BM_add_unordered_map)->RangeMultiplier(10)->Range(10, 100'000);
BENCHMARK(BM_add_raw_hash_map)->RangeMultiplier(10)->Range(10, 100'000);
BENCHMARK(BM_find_unordered_map)->RangeMultiplier(10)->Range(10, 100'000);
BENCHMARK(BM_find_raw_hash_map)->RangeMultiplier(10)->Range(10, 100'000);

BENCHMARK_MAIN();
