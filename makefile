# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -O2

# Default build target
all: aig

# Build the main program (aig)
aig: src/main.cpp src/aig.cpp
	$(CXX) $(CXXFLAGS) -o ./aig src/main.cpp src/aig.cpp

.PHONY: main # special target in a Makefile that tells make the target is not a real file
main:
	./aig

.PHONY: test
test: run_tests
	@echo "Running unit tests..."
	@./run_tests

# Build test binary
run_tests: src/aig.cpp tests/test_aig.cpp
	$(CXX) $(CXXFLAGS) -o ./run_tests src/aig.cpp tests/test_aig.cpp

# Clean up all generated binaries
.PHONY: clean
clean:
	rm -f ./aig ./run_tests
