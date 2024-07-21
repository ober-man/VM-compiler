# Optimizing compiler passes
This directory contains [PassManager](https://github.com/ober-man/VM-compiler/blob/main/pass/passmanager.h) and some [passes](https://github.com/ober-man/VM-compiler/blob/main/pass/pass.h) that it runs.
There are 2 types of passes:
- Analysis - do not change the graph, just collect some data for optimizations
- Optimization - can change graph, making more optimal code

## Analysis
- [RPO](https://github.com/ober-man/VM-compiler/blob/main/pass/rpo.h), Reverse Post-Order - a graph order, guaranteed that all predecessors has been visited before current node is visited
- [Dominators Tree](https://github.com/ober-man/VM-compiler/blob/main/pass/domtree.h) - finding each node dominators
- [Loops Analysis](https://github.com/ober-man/VM-compiler/blob/main/pass/loop_analysis.h) - finding all graph loops
- [Linear Order](https://github.com/ober-man/VM-compiler/blob/main/pass/linear_order.h) - a graph order, which stride it to a line with minimal amount of above branches
- [Liveness Analysis](https://github.com/ober-man/VM-compiler/blob/main/pass/liveness.h) - defining all variables lifetime interval from the definition to the last use

## Optimization
- [Checks Elimination](https://github.com/ober-man/VM-compiler/blob/main/pass/checks_elimination.h) - remove redundant dominated checks
- [Const Folding](https://github.com/ober-man/VM-compiler/blob/main/pass/const_folding.h) - replace an operation with constants with evaluated constant
- [DCE](https://github.com/ober-man/VM-compiler/blob/main/pass/dce.h), Dead Code Elimination - remove dead and unreachable code
- [Inline](https://github.com/ober-man/VM-compiler/blob/main/pass/inline.h) - substitute a function body to the point of this function call
- [Peepholes](https://github.com/ober-man/VM-compiler/blob/main/pass/peepholes.h) - some local optimizations of binary/unary instructions
- [RegAlloc](https://github.com/ober-man/VM-compiler/blob/main/pass/reg_alloc.h) - allocating physical registers for all variables