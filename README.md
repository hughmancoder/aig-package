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
- [ ] Bliff support (write_aig_blif) (very easy now we have boolean generation)
- [ ] Import / Parse bliff
- [ ] Export / Save to bliff
- [ ] Binary AIG Support (read_aiger_binary)
- [ ] Depth / fan-out analysis 


### Goals + Extensions

- [ ] Structural hashing (replace naive push_back with a unique-table)
- [ ] Very basic optmisation
- [ ] Graph export/print and pretty printing (Hard)
