# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -O2

# Default build target
all: aig

## Helper macro

ifndef FILE
	FILE := examples/and2.aag   # default
endif

# Build the main program (aig)
aig: src/main.cpp src/aig.cpp
	$(CXX) $(CXXFLAGS) -o ./aig src/main.cpp src/aig.cpp

.PHONY: main # special target in a Makefile that tells make the target is not a real file
main:
	./aig

write_to_aag: aig
	@mkdir -p build
	@./aig write $(FILE) $(OUT)
	@echo "Wrote $(OUT)"

truth_table: aig
	@./aig truth_table $(FILE)

show_stats: aig
	@./aig stats $(FILE)

.PHONY: test
test: run_tests
	@./run_tests

# builds test binary
run_tests: src/aig.cpp tests/test_aig.cpp
	$(CXX) $(CXXFLAGS) -o ./run_tests src/aig.cpp tests/test_aig.cpp

.PHONY: clean
clean:
	rm -f ./aig ./run_tests
