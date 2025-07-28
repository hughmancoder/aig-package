# AIG package

Hugh Signoriello

## AIG (And-Inverter Graph)

A simple And-Inverter Graph library in C++ for representing and manipulating Boolean functions

## Features

- Parses AIGER files (ASCII and binary) .aag format
- Validates if constructed AIG is valid (is_topologically_sorted)
- Generate Truth Table from constructed aig node
- Show AIG node stats (Inputs, Outputs, And Gates, Depth, max Fan Out)
- Write to AIG to AIGER .aag file

[Library Notes](/documentation/NOTES.md)

[ASCII AIG Format representation](/documentation/ASCII_AIGER_FORMAT.md)

[AIG format and AIG construction examples](/documentation/EXAMPLES.md)

## Prerequisites

- A C++17 compiler (GCC or Clang)
- Make (or CMake, if you add support later)

## Usage

```shell
# construct aig from aag file show stats
make show_stats FILE=<path/to/file.aag> 

# construct aig from aag generate truth table
make truth_table FILE=<path/to/file.aag>

# Write aig to .aag 
make write_to_aag FILE=<path/to/input_file.aag> OUT=<path/to/output_file.aag>


make         # builds ./aig
make test    # builds and runs ./run_tests
make clean   # deletes ./aig and ./run_tests
```

## Examples

```bash
make show_stats FILE=examples/full_adder.aag
make truth_table FILE=examples/full_adder.aag
make write_to_aag FILE=examples/full_adder.aag OUT=examples/outputs/full_adder_output.

make show_stats FILE=examples/and2.aag
make truth_table FILE=examples/and2.aag

make show_stats FILE=examples/and2_redundant.aag
make truth_table FILE=examples/and2_redundant.aag
```
