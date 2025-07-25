# Notes

## And inverter graph

An AIG (And Inverter Graph) is a directed acyclic graph used to represent Boolean functions and consists of nodes that represent logical operations. Each node consists of two inputs and one output, where the inputs can be either inverted or non-inverted.
 
Combining these nodes allows us to represent more complex logic as Nand gates can be combined in different ways to create any Boolean function.

## AIG Node format (ASCII AIGER)

An AIG node is represented by a literal, which is an integer that can be either even or odd. It encodes both the node number and whether the node is inverted or not

- Literal = node number * 2 + inversion bit = ID
- Even Literals: positive non inverted node
- Odd Literals: inverted node (Inversion bit = 1)

Suppose L is a literal, then:
Node number = L // 2 (same as L >> 1)
Inversion bit = L % 2

Example:
Literal 6: Node 3, normal
Literal 7: Node 3, inverted

### Canonical Literal

A canonical literal is the even version of a literal, which is used to represent the node ID without the inversion bit. This allows us to group nodes that are logically equivalent. It can be thought of as a group id for the node.

L & ~1u (removes the inversion bit)

```plaintext
lit=2  binary=010  => canonical=2  bin=010
lit=3  binary=011  => canonical=2  bin=010

lit=4  binary=100  => canonical=4  bin=100
lit=5  binary=101  => canonical=4  bin=100
```

### Constant Literrals

Literal 0 → constant FALSE
Literal 1 → constant TRUE


## Mapping literals to node Ids

Example: 
| id (literal) | Binary | `id = L >> 1` |
| ------------ | ------ | ---------- |
| 4            | `100`  | `010` (2)  |
| 5            | `101`  | `010` (2)  |

This allows us to group up literals that belong to the same node ID



<https://fmv.jku.at/aiger/FORMAT.aiger>

aag file format is a text format for AIGER files.

This is human readable and can be used to create AIGER files. it has .aag extension

## And example

```and2.aag
aag 3 2 0 1 1
2 = A (input 1)
4 = B (input 2)
6 = Y (output)
6 2 4 (Y = A & B)
```

The first line is the header which follows MILOA format.

```text
M=3: highest variable index is 3

I=2: two primary inputs

L=0: no latches

O=1: one primary output

A=1: one AND gate

For the line 6 2 4

Node n;

n.id = 6;

n.fanin0 = 2;

n.fanin1 = 4;
```

This encodes the AND gate operation, where 6 is the node id, and 2 and 4 are the input nodes. Because they are even they are non inverted and 2 gives node number 2 / 2 = 1 (A) and 4 gives node number 4 / 2 = 2 (B).

The order of this encoding is node output, input1, input2. Hence Y = A & B,

Encoding enuerated

| Node # | Meaning           | Literal |
| ------ | ----------------- | ------- |
| 0      | false const       | `0`     |
| 1      | true  const (=¬0) | `1`     |
| 2      | A                 | `2`     |
| 3      | ¬A                | `3`     |
| 4      | B                 | `4`     |
| 5      | ¬B                | `5`     |
| 6      | A·B               | `6`     |
| 7      | ¬(A·B)            | `7`     |

### Full adder example

A more complex aig representation is a full adder. The logic for a full adder is demonstrated in the truth table below:

<!-- ![Full Adder Truth Table](../Images/Full_adder_truth_table.png) -->

![Full Adder Diagram](../Images/full_adder_aig_diagram.png)

### Full Adder Truth Table

| A | B | Cin | Sum | Cout |
|---|---|-----|-----|------|
| 0 | 0 |  0  |  0  |  0   |
| 0 | 0 |  1  |  1  |  0   |
| 0 | 1 |  0  |  1  |  0   |
| 0 | 1 |  1  |  0  |  1   |
| 1 | 0 |  0  |  1  |  0   |
| 1 | 0 |  1  |  0  |  1   |
| 1 | 1 |  0  |  0  |  1   |
| 1 | 1 |  1  |  1  |  1   |

This can be represented in AIGER format as follows

```full_adder.aag
aag 14 3 0 2 11
2
4
6
17
29
8 2 4
10 2 6
12 4 6
14 9 11
16 14 13
18 2 5
20 3 4
22 19 21
24 23 7
26 22 6
28 25 27
c
full_adder example (carry, sum)
```

The image below explains the AAG representation of the full adder, and how each line in the aag file encodes a node to represent the full adder logic.

![Full Adder AAG Explained](../Images/full_adder_aag_explained.png)

## enumerate_inputs example

We can generate ordered truth table from inputs by counting from 0 to 2^n - 1 and enumerating the binary.

e.g for 2 inputs, we have 2^2 = 4 rows, and the truth table is:
This logic (i >> bit) & 1u extracts the bit at each position for each row

```plaintext
i = 0 (binary 00):
  bit 1: (0 >> 1) & 1 = 0
  bit 0: (0 >> 0) & 1 = 0
  row pushed: {0, 0}

i = 1 (binary 01):
  bit 1: (1 >> 1) & 1 = 0
  bit 0: (1 >> 0) & 1 = 1
  row pushed: {0, 1}

i = 2 (binary 10):
  bit 1: (2 >> 1) & 1 = 1
  bit 0: (2 >> 0) & 1 = 0
  row pushed: {1, 0}

i = 3 (binary 11):
  bit 1: (3 >> 1) & 1 = 1
  bit 0: (3 >> 0) & 1 = 1
  ```

  ### evaluate_aig example

![Evaluate AIG and example](../Images/evaluate_aig_and_example.png)

## BLIF (Berkeley Logic Interchange Format)

<https://course.ece.cmu.edu/~ee760/760docs/blif.pdf>
