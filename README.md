# Virtual Machine Compiler

JIT & AOT optimizing compiler middle-end and backend

## Project structure
- [ir](https://github.com/ober-man/VM-compiler/tree/main/ir)     - Compiler Intermediate Representation (IR)
- [doc](https://github.com/ober-man/VM-compiler/tree/main/doc)    - Documentation for compiler IR and its passes
- [pass](https://github.com/ober-man/VM-compiler/tree/main/pass)   - PassManager and its passes 
- [test](https://github.com/ober-man/VM-compiler/tree/main/tests)   - Functionality tests

Now compiler support next list of optimizations:
- Checks Elimination
- Const Folding
- Dead Code Elimination (DCE)
- Inline
- Peepholes
- Register Allocation (RegAlloc)

For detailed description look corresponding doc file.

## How to build
```sh
mkdir build
cd build
cmake ..
cmake --build .
```

## How to run tests
```sh
cd build
ctest [-VV]
```