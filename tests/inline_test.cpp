#include "ir/graph.h"
#include "pass/inline.h"
#include "gtest/gtest.h"

using namespace compiler;

/**
 * Caller graph:
 *                 [1]
 *                  |
 *                  v
 *                 [2] <- call
 *                  |
 *                  v
 *                 [3]
 *
 * Callee graph:
 *                 [1]
 *                  |
 *                  v
 *             /---[2]---\
 *             |         |
 *             v         v
 *            [3]       [4]
 *             |         |
 *             \-->[5]<--/
 */
TEST(INLINE_TEST, TEST1)
{
    /*
    Graph for proc caller
    BB [1/3]
        v0. Param i64 a
        v1. Const i64 2
    BB [2/3]
        v2. Mul v0, v1
        v3. Call func
    BB [3/3]
        v4. Ret i64 v3
    */

    /*
    Graph for proc callee
    BB [1/5]
        v0. Param i64 x
        v1. Const i64 0

    BB [2/5]
        v2. Cmp   i64 v0, v1
        v3. Ja    bb4

    BB [3/5]
        v4. Add   i64 v1, v0
        v5. Ret   i64 v4

    BB [4/5]
        v6. Sub   i64 v1, v0
        v7. Ret   i64 v6

    BB [5/5]
        empty
    end
    */
    Graph* graph1_ptr = new Graph{"callee"};
    std::shared_ptr<Graph> graph1(graph1_ptr);

    auto* bb11 = new BasicBlock{1, graph1};
    auto* bb12 = new BasicBlock{2, graph1};
    auto* bb13 = new BasicBlock{3, graph1};
    auto* bb14 = new BasicBlock{4, graph1};
    auto* bb15 = new BasicBlock{5, graph1};

    graph1->insertBB(bb11);
    graph1->insertBB(bb12);
    graph1->insertBBAfter(bb12, bb13, true);
    graph1->insertBBAfter(bb12, bb14, false);
    graph1->insertBBAfter(bb13, bb15);
    graph1->addEdge(bb14, bb15);

    auto* v10 = new ParamInst{0, DataType::i64, "x"};
    auto* v11 = new ConstInst{1, static_cast<uint64_t>(0)};
    bb11->pushBackInst(v10);
    bb11->pushBackInst(v11);

    auto* v12 = new BinaryInst{2, InstType::Cmp, v10, v11};
    auto* v13 = new JumpInst{3, InstType::Ja, bb14};
    bb12->pushBackInst(v12);
    bb12->pushBackInst(v13);

    auto* v14 = new BinaryInst{4, InstType::Add, v11, v10};
    auto* v15 = new UnaryInst{5, InstType::Return, v14};
    bb13->pushBackInst(v14);
    bb13->pushBackInst(v15);

    auto* v16 = new BinaryInst{6, InstType::Sub, v11, v10};
    auto* v17 = new UnaryInst{7, InstType::Return, v16};
    bb14->pushBackInst(v16);
    bb14->pushBackInst(v17);
    // graph1->dump();

    ////////////////////////////////////////////////////////////

    auto graph2 = std::make_shared<Graph>("caller");

    auto* bb21 = new BasicBlock{1, graph2};
    auto* bb22 = new BasicBlock{2, graph2};
    auto* bb23 = new BasicBlock{3, graph2};

    graph2->insertBB(bb21);
    graph2->insertBB(bb22);
    graph2->insertBB(bb23);

    auto* v20 = new ParamInst{0, DataType::i64, "a"};
    auto* v21 = new ConstInst{1, static_cast<uint64_t>(2)};
    bb21->pushBackInst(v20);
    bb21->pushBackInst(v21);

    auto* v22 = new BinaryInst{2, InstType::Mul, v20, v21};
    auto* v23 = new CallInst{3, graph1_ptr, {v22}};
    bb22->pushBackInst(v22);
    bb22->pushBackInst(v23);

    auto* v24 = new UnaryInst{4, InstType::Return, v23};
    bb23->pushBackInst(v24);
    // graph2->dump();

    ////////////////////////////////////////////////////////////

    graph2->runPass<Inline>();
    // graph2->dump();
    ASSERT_EQ(graph2->size(), 8);
    ASSERT_EQ(static_cast<ConstInst*>(bb21->getLastInst())->getIntValue(), 2);
    ASSERT_EQ(static_cast<BinaryInst*>(bb12->getFirstInst())->getInput(0), v22);
}
