
# AIGER ASCII Format (.aag)

<https://fmv.jku.at/aiger/FORMAT.aiger>

An AIG node is represented by a literal, which is an integer that can be either even or odd. It encodes both the node number and whether the node is inverted or not

```text
aag M I L O A
< I input literals >
< L latch lines >
< O output literals >
< A AND-gate lines: one per gate >
```

- M: highest variable index (max node id)

- I: number of primary inputs

- L: number of latches (usually 0)

- O: number of primary outputs

- A: number of AND gates
  
Important for ASCII: The order of AND definitions is not guaranteed to be topological

## Literals

A literal is a 32-bit integer encoding both node index and inversion

## Literals vs variable indices

A literal encodes both a variable index and phase (inversion bit) for an aig node
literal = 2 * var_index + inversion_bit
var_index = literal >> 1 (equivalent to dividing by 2 and flooring)
inversion_bit = literal & 1 (checks if odd)
Constant literals: 0 = FALSE, 1 = TRUE

Example

```plaintext
Literal 6: var_index = 6 >> 1 = 3, phase = 110 & 001 = 0 = non-inverted 
Literal 7: var_index = 7 >> 1 = 3, phase = 111 & 001 = 1 = inverted
```

### Canonical Literal

`Canonical literal = L & ~1u (clear the phase bit ⇒ always even)`

- We us it whenever you want to treat the signal independent of its phase/inversion_bit (i.e., x and !x are the same node)
- This allows us to group nodes that are logically equivalent. It can be thought of as a group id for the node.
- We use when  we look up or store groups in a map e.g: unordered_map<Lit,AigNode>
  
This is useful to group up the signal for:

- Fan-out counting
- Memoized depth
- Structural hashing

Example

```plaintext
lit=2  binary=010  => canonical=2  bin=010
lit=3  binary=011  => canonical=2  bin=010

lit=4  binary=100  => canonical=4  bin=100
lit=5  binary=101  => canonical=4  bin=100
```
