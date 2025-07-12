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
Node number = L // 2
Inversion bit = L % 2

Example:
Literal 6: Node 3, normal
Literal 7: Node 3, inverted

<https://fmv.jku.at/aiger/FORMAT.aiger>

aag file format is a text format for AIGER files.

This is human readable and can be used to create AIGER files. it has .aag extension

## And example

```and.aag
aag 3 2 0 1 1
2 = A (input 1)
4 = B (input 2)
6 = Y (output)
6 2 4 (Y = A & B)
```

The first line is the header which follows MILOA format.

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

This encodes the AND gate operation, where 6 is the node id, and 2 and 4 are the input nodes. Because they are even they are non inverted and 2 gives node number 2 / 2 = 1 (A) and 4 gives node number 4 / 2 = 2 (B).

The order of this encoding is node output, input1, input2. Hence Y = A & B,

### Full adder example

A more complex aig representation is a full adder. The logic for a full adder is demonstrated in the truth table below:

![Full Adder Truth Table](../Images/Full_adder_truth_table.png)


![Full Adder Diagram](../Images/Full%20Adder%20Diagran.png)

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