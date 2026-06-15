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

static constexpr size_t SAMPLES   = 5'000'000;
static constexpr size_t SEED      = 12345654321;
static const Corpus     g_corpus  = load_corpus("text.txt");
static constexpr auto   EMPTY_STR = "__EMPTY__";
static constexpr auto   EMPTY_SV  = std::string_view{EMPTY_STR};
static volatile int     sink;

Histogram hist_find_uint32_t_raw(size_t n);
Histogram hist_find_uint32_t_google(size_t n);
Histogram hist_insert_uint32_t_google(size_t n);
Histogram hist_insert_uint32_t_raw(size_t n);
Histogram hist_find_string_view_raw(size_t n);
Histogram hist_find_string_view_google(size_t n);
Histogram hist_insert_string_view_google(size_t n);
Histogram hist_insert_string_view_raw(size_t n);

#endif // !__HIST_FUNCTIONS__H_
