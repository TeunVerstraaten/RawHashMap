#!/usr/bin/env bash

set -euo pipefail

echo "[1/2] Compiling..."
g++ -std=c++17 src/tests.cpp -lgtest -lgtest_main -lpthread -o my_tests.out

echo "[2/2] Running tests..."
./my_tests.out

echo "Done"
