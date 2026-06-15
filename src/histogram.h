#ifndef __HISTOGRAM__H_
#define __HISTOGRAM__H_

#include <array>
#include <cstddef>

template <size_t N>
constexpr std::array<size_t, N> arithmetic_sequence(size_t a, size_t diff) {
    std::array<size_t, N> result{};

    for (std::size_t i = 0; i < N; ++i) {
        result[i] = a + diff * i;
    }

    return result;
}

class Histogram {
  public:
    static constexpr std::size_t N       = 15;
    static constexpr auto        BUCKETS = arithmetic_sequence<N>(20, 10);

  private:
    std::array<size_t, N> _counts = {0};

  public:
    void observe(double v) {
        for (size_t i = 0; i != N; ++i) {
            if (BUCKETS[i] >= v) {
                ++_counts[i];
                return;
            }
        }
        ++_counts.back();
    }

    const std::array<size_t, N>& counts() const {
        return _counts;
    }
};

#endif // !__HISTOGRAM__H_
