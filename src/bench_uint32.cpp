#include "raw_hash_map.h"

#include <benchmark/benchmark.h>
#include <cctype>
#include <cstdint>
#include <random>

static constexpr size_t SEED = 12344321;

template <typename Map>
static void BM_add(benchmark::State& state) {
    size_t       n = static_cast<size_t>(state.range(0));
    std::mt19937 rng(SEED);
    size_t       its = 0;

    Map map;
    map.reserve(n);

    for (auto _ : state) {
        size_t c = 0;

        for (size_t i = 0; i < n; i++) {
            map[rng()] += 1;
        }
        benchmark::DoNotOptimize(c);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * n);
}

static void BM_add_unordered_map(benchmark::State& state) {
    BM_add<std::unordered_map<uint32_t, size_t>>(state);
}
static void BM_add_raw_hash_map(benchmark::State& state) {
    BM_add<RawHash<uint32_t, size_t>>(state);
}

static void BM_find_unordered_map(benchmark::State& state) {
    size_t       n = static_cast<size_t>(state.range(0));
    std::mt19937 rng(SEED);
    size_t       its = 0;

    std::unordered_map<uint32_t, size_t> map;
    map.reserve(n);
    auto rngs = std::vector<uint32_t>{};
    rngs.reserve(n);

    for (size_t i = 0; i < n; i++) {
        rngs.push_back(rngs[i]);
    }
    for (size_t i = 0; i < n; i++) {
        map[rng()] += 1;
    }

    for (auto _ : state) {
        size_t c = 0;

        for (size_t i = 0; i < n; i++) {
            auto f = map.find(rngs[i]);
            if (f != map.end()) {
                c += f->second;
            }
        }

        benchmark::DoNotOptimize(c);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * n);
}

static void BM_find_raw_hash_map(benchmark::State& state) {
    size_t       n = static_cast<size_t>(state.range(0));
    std::mt19937 rng(SEED);
    size_t       its = 0;

    RawHash<uint32_t, size_t> map;
    map.reserve(n);
    auto rngs = std::vector<uint32_t>{};
    rngs.reserve(n);

    for (size_t i = 0; i < n; i++) {
        rngs.push_back(rngs[i]);
    }
    for (size_t i = 0; i < n; i++) {
        map[rng()] += 1;
    }

    for (auto _ : state) {
        size_t c = 0;

        for (size_t i = 0; i < n; i++) {
            auto f = map.get(rngs[i]);
            if (f) {
                c += *f;
            }
        }

        benchmark::DoNotOptimize(c);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// BENCHMARK(BM_add_unordered_map)->RangeMultiplier(10)->Range(10, 100'000);
// BENCHMARK(BM_add_raw_hash_map)->RangeMultiplier(10)->Range(10, 100'000);
BENCHMARK(BM_find_unordered_map)->RangeMultiplier(10)->Range(10, 100'000);
BENCHMARK(BM_find_raw_hash_map)->RangeMultiplier(10)->Range(10, 100'000);

BENCHMARK_MAIN();
