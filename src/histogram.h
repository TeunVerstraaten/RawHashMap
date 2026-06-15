#ifndef __HISTOGRAM__H_
#define __HISTOGRAM__H_

#include <array>
#include <cstddef>

class Histogram {
  public:
    static constexpr size_t                N       = 14;
    static constexpr std::array<double, N> BUCKETS = {
        20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0, 100.0, 120.0, 140.0, 160.0, 180.0, 200.0};

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

    const std::array<size_t, N> counts() const {
        return _counts;
    }
};

#endif // !__HISTOGRAM__H_
