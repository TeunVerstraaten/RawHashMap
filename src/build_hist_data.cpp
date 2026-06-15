#include "hist_functions.h"
#include "histogram.h"
#include "raw_hash_map.h"

#include <sparsehash/dense_hash_map>
#include <string_view>

void build_hist_data(std::string                                   title,
                     size_t                                        min,
                     size_t                                        max,
                     std::vector<std::string>                      bench_names,
                     std::vector<std::function<Histogram(size_t)>> benches) {
    std::ofstream outfile(title + ".dat");
    assert(outfile.is_open());

    outfile << title << "\n\n";
    for (size_t n = min; n <= max; n *= 10) {
        std::vector<Histogram> results{};
        for (auto& f : benches) {
            results.push_back(f(n));
        }

        outfile << "n_elements: " << n << "\n";
        outfile << "n ";
        for (const auto& name : bench_names) {
            outfile << name << " ";
        }
        outfile << "\n";
        for (size_t i = 0; i != Histogram::N; ++i) {
            outfile << Histogram::BUCKETS[i] << " ";
            for (const auto& h : results) {
                outfile << h.counts()[i] << " ";
            }
            outfile << "\n";
        }
        outfile << "\n";
    }
}

int main() {
    size_t min = 10;
    size_t max = 1'000'000;

    build_hist_data("find__string_view", min, max, {"raw", "google"}, {hist_find_raw_string_view, hist_find_google_string_view});
    build_hist_data("find__uint32_t", min, max, {"raw", "google"}, {hist_find_raw_uint32_t, hist_find_google_uint32_t});
    build_hist_data("add__string_view",
                    min,
                    max,
                    {"raw", "google"},
                    {hist_add_string_view<RawHashMap<std::string_view, size_t>>, hist_add_string_view_google});
    build_hist_data("add__uint32_t",
                    min,
                    max,
                    {"raw", "google"},
                    {hist_add_uint32_t<RawHashMap<uint32_t, size_t>>, hist_add_uint32_t_google});
}
