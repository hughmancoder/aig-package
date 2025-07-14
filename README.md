# AIG package

Hugh Signoriello

## AIG (And-Inverter Graph)

Library for representing AIGs (And-Inverter Graphs)

## Usage

```shell
make         # builds ./aig
make main    # runs ./aig
make test    # builds and runs ./run_tests
make clean   # deletes ./aig and ./run_tests
```

## TODO

### Core
- [x] AIG Node class
- [x] AIG.hpp (node)
- [x] Parsing AAG (ASCII)
- [x] Verify counts & topological order
- [x] Resolve TODOS
- [x] Test and.aig (check inputs, ouputs, fan-ins of gates)
- [ ] Binary AIG Support (read_aiger_binary)
- [ ] Graph export/print and pretty printing

### Goals + Extensions

- [ ] Structural hashing (replace naive push_back with a unique-table)
- [ ] ASCII parser (read_aiger_ascii) / binary support (read_aiger_binary)
- [ ] Tiny optimisation pass
- [ ] Depth / fan-out analysis 
- [ ] Technology-mapping stub (BLIF or Verilog export)

 for breakpoint-friendly debugging

Depth / fan-out analysis – trivial once you have DFS utilities

