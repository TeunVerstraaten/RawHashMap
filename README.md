A fast hashmap implementation in c++.

To generate the histograms run the following command

```
g++ src/build_hist_data.cpp -O3 -march=native -std=c++17 && taskset -c 3 ./a.out && python3.12 plot_hist.py
```
