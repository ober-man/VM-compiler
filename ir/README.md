# Intermediate Representation (IR)

Graph-based IR for further optimizations and analyses passes on it. 

## Graph 
A standard [graph](https://github.com/ober-man/VM-compiler/blob/main/ir/graph.h) representation, where nodes are Basic Blocks. Represents both Contol FLow Graph (CFG) and Data Flow Graph (DFG). Contains a vector of BBs with unique id.
Some graph helpers:
- [MarkerManager](https://github.com/ober-man/VM-compiler/blob/main/ir/marker.h) - a helper class to mark visited graph nodes while some kind of processing. The main its aim is to prevent process looping. Support some simultaneously graph processing. 
- [PassManager](https://github.com/ober-man/VM-compiler/blob/main/pass/passmanager.h) - a helper class to run passes and optimizations.

## Basic Block
[Basic Block](https://github.com/ober-man/VM-compiler/blob/main/ir/basicblock.h) (BB) is a linear sequence of instructions with no enter except the first instruction and no exit except the last instruction.
Each BB can have 1 or 2 successors and any number of predecessors. Edges represents CFG edges.
BB contains instructions. Instructions are designed as a double-linked intrusive list with knowing next and prev instructions.

## Instruction
[Instruction](https://github.com/ober-man/VM-compiler/blob/main/ir/inst.h) has dataflow users and operands.
Instructions structure:
- BinaryInst: arithmetic Add/Sub/Mul/Div, bitwise Shl/Shr, logic And/Or/Xor, compare Cmp
- UnaryInst: Neg, Not, Return
- ConstInst: i32/i64/f32/f64 constant. Constants are located at the first BB for convenience
- ParamInst: i32/i64/f32/f64 function parameter. Params are also located at the first BB
- JumpInst: conditional Je/Jne/Ja/Jb or unconditional Jmp instruction
- CallInst: Call function with dynamic operands list
- PhiInst: Phi function to choose correct operand depend on previous control-flow branch

## Example
```
                 [1]
                  |
                  v
             /---[2]---\
             |         |
             v         v
            [3]------>[4]
 ```
 ```
auto *graph = new Graph{'testGraph'};

auto* bb1 = new BasicBlock{1, graph};
auto* bb2 = new BasicBlock{2, graph};
auto* bb3 = new BasicBlock{3, graph};
auto* bb4 = new BasicBlock{4, graph};

graph->insertBB(bb1);
graph->insertBB(bb2);
graph->insertBB(bb3);
graph->insertBB(bb4);
graph->addEdge(bb2, bb4);

auto* v0 = new ParamInst{0, DataType::i32, "a0"};
auto* v1 = new ConstInst{1, static_cast<uint64_t>(0)};
bb1->pushBackInst(v0);
bb1->pushBackInst(v1);

auto* v2 = new BinaryInst{2, InstType::Cmp, v0, v1};
auto* v3 = new JumpInst{3, InstType::Ja, bb4};
bb2->pushBackInst(v2);
bb2->pushBackInst(v3);

auto* v4 = new BinaryInst{4, InstType::Add, v1, v0};
auto* v5 = new JumpInst{5, InstType::Jmp, bb4};
bb3->pushBackInst(v4);
bb3->pushBackInst(v5);

auto* v6 = new PhiInst{6, {v1, bb2}, {v4, bb3});
auto* v7 = new UnaryInst{7, InstType::Return, v6};
bb4->pushBackPhiInst(v6);
bb4->pushBackInst(v7);
```

## Literature:
- A Simple [Graph-Based](https://www.oracle.com/technetwork/java/javase/tech/c2-ir95-150110.pdf) Intermediate Representation 
- Semantic Reasoning about the [Sea of Nodes](https://dl.acm.org/doi/10.1145/3178372.3179503)
- [LLVM](https://llvm.org/pubs/2004-01-30-CGO-LLVM.pdf): A Compilation Framework for Lifelong Program Analysis & Transformation
- [MLIR](https://arxiv.org/pdf/2002.11054): A Compiler Infrastructure for the End of Moore’s Law