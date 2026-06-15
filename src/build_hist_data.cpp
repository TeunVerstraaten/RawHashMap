#include "hist_functions.h"
#include "histogram.h"

#include <cassert>
#include <fstream>
#include <functional>

constexpr size_t MIN = 10;
constexpr size_t MAX = 1'000'000;

void build_hist_data(std::string                                   title,
                     std::vector<std::string>                      bench_names,
                     std::vector<std::function<Histogram(size_t)>> benches) {
    std::ofstream outfile(title + ".dat");
    assert(outfile.is_open());

    outfile << title << "\n\n";
    for (size_t n = MIN; n <= MAX; n *= 10) {
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
    build_hist_data("find__string_view", {"raw", "google"}, {hist_find_string_view_raw, hist_find_string_view_google});
    build_hist_data("find__uint32_t", {"raw", "google"}, {hist_find_uint32_t_raw, hist_find_uint32_t_google});
    build_hist_data("remove__string_view", {"raw", "google"}, {hist_remove_string_view_raw, hist_remove_string_view_google});
    build_hist_data("remove__uint32_t", {"raw", "google"}, {hist_remove_uint32_t_raw, hist_remove_uint32_t_google});
    build_hist_data("insert__string_view", {"raw", "google"}, {hist_insert_string_view_raw, hist_insert_string_view_google});
    build_hist_data("insert__uint32_t", {"raw", "google"}, {hist_insert_uint32_t_raw, hist_insert_uint32_t_google});
}
