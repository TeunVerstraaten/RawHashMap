#!/usr/bin/env bash

set -euo pipefail

echo "[1/3] Compiling..."
g++ src/build_hist_data.cpp -O3 -march=native -std=c++17 -o build_hist_data.out

echo "[2/3] Generating data..."
taskset -c 3 ./build_hist_data.out

echo "[3/3] Plotting..."
python3.12 plot_hist.py

echo "Done."
