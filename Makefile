CXX = g++
CXXFLAGS = -std=c++17 -Wall -Werror -pedantic -O3 -march=native

HIST_BIN = build_hist_data.out
TEST_BIN = my_tests.out
BENCH_BIN = bench.out

.PHONY: all
all: hist test

.PHONY: hist 

hist: 
	@echo "[1/3] Compiling histogram tool..."
	@$(CXX) src/build_hist_data.cpp src/corpus.cpp src/hist_functions.cpp $(CXXFLAGS) -o $(HIST_BIN) 
	@echo "[2/3] Generating data histogram data..."
	@taskset -c 3 ./$(HIST_BIN)
	@echo "[3/3] Plotting histograms..."
	@python3.12 plot_hist.py

.PHONY: bench 

bench: 
	@echo "[1/2] Compiling bench..."
	@$(CXX) src/bench_string_view.cpp src/corpus.cpp src/hist_functions.cpp $$(pkg-config --cflags --libs benchmark) $(CXXFLAGS) -o $(BENCH_BIN) 
	@echo "[2/2] Running bench..."
	@./$(BENCH_BIN)

.PHONY: test 

test: 
	@echo "[1/2] Compiling tests..."
	@$(CXX) src/tests.cpp -lgtest -lgtest_main -lpthread -o $(TEST_BIN)
	@echo "[2/2] Running tests..."
	@./$(TEST_BIN)

.PHONY: clean
clean:
	rm -f $(HIST_BIN) $(TEST_BIN) $(BENCH_BIN)
