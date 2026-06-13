#include "hist_functions.h"
#include "histogram.h"
#include "raw_hash_map.h"

#include <string_view>
#include <unordered_map>

int main() {
    size_t min = 10;
    size_t max = 1'000'000;

    {
        std::ofstream outfile("find__string_view.dat");
        assert(outfile.is_open());

        outfile << "find__string_view\n\n";
        for (size_t n = min; n <= max; n *= 10) {
            auto raw   = hist_find_raw_string_view(n);
            auto unord = hist_find_unordered_map_string_view(n);

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
        std::ofstream outfile("add__string_view.dat");
        assert(outfile.is_open());

        outfile << "add__string_view\n\n";
        for (size_t n = min; n <= max; n *= 10) {
            auto raw   = hist_add_string_view<RawHashMap<std::string_view, size_t>>(n);
            auto unord = hist_add_string_view<std::unordered_map<std::string_view, size_t>>(n);

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
        std::ofstream outfile("find__uint32_t.dat");
        assert(outfile.is_open());

        outfile << "find__uint32_t\n\n";
        for (size_t n = min; n <= max; n *= 10) {
            auto raw   = hist_find_raw_uint32_t(n);
            auto unord = hist_find_unordered_map_uint32_t(n);

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
        std::ofstream outfile("add__uint32_t.dat");
        assert(outfile.is_open());

        outfile << "add__uint32_t\n\n";
        for (size_t n = min; n <= max; n *= 10) {
            auto raw   = hist_add_uint32_t<RawHashMap<uint32_t, size_t>>(n);
            auto unord = hist_add_uint32_t<std::unordered_map<uint32_t, size_t>>(n);

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
