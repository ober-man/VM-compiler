#include "ir/graph.h"
#include "pass/checks_elimination.h"
#include "gtest/gtest.h"

using namespace compiler;

TEST(CHECKS_ELIMINATION_TEST, TEST1)
{
    /*
    Graph for proc (preds, succs omitted)
    BB [1/2]
        v0. Param i64 a0
        v1. Const i64 25

    BB [2/2]
        v2. ZeroCheck i64 v0
        v3. Div   i64 v1, v0
        v4. ZeroCheck i64 v0 -- dominated by v2
        v5. Div   i64 v3, v0
        v6. Ret   i64 v5
    end
    */
    auto graph = std::make_shared<Graph>("checks_elimination_test1");

    auto* bb1 = new BasicBlock{1, graph};
    auto* bb2 = new BasicBlock{2, graph};

    graph->insertBB(bb1);
    graph->insertBB(bb2);

    auto* v0 = new ParamInst{0, DataType::i64, "a0"};
    auto* v1 = new ConstInst{1, static_cast<uint64_t>(25)};
    bb1->pushBackInst(v0);
    bb1->pushBackInst(v1);

    auto* v2 = new UnaryInst{2, InstType::ZeroCheck, v0};
    auto* v3 = new BinaryInst{3, InstType::Div, v1, v0};
    auto* v4 = new UnaryInst{4, InstType::ZeroCheck, v0};
    auto* v5 = new BinaryInst{5, InstType::Div, v3, v0};
    auto* v6 = new UnaryInst{6, InstType::Return, v5};
    bb2->pushBackInst(v2);
    bb2->pushBackInst(v3);
    bb2->pushBackInst(v4);
    bb2->pushBackInst(v5);
    bb2->pushBackInst(v6);

    // graph->dump();
    graph->runPass<ChecksElimination>();
    // graph->dump();
    ASSERT_EQ(v3->getNext(), v5);
    ASSERT_EQ(v5->getPrev(), v3);
}

/**
 * Test2 graph:
 *                 [1]
 *                  |
 *                  v
 *             /---[2]---\
 *             |         |
 *             v         v
 *            [3]------>[4]
 */
TEST(CHECKS_ELIMINATION_TEST, TEST2)
{
    /*
    Graph for proc (preds, succs omitted)
    BB [1/4]
        v0. Param i64 a0
        v1. Const i64 0
        v100. Const i64 UINT_MAX

    BB [2/4]
        v2. ZeroCheck i64 v0
        v3. Div   i64 v1, v0
        v4. BoundsCheck i64 v0, v100
        v5. Mul   i64 v3, v0
        v55. Cmp i64 v5, v0
        v6. Ja    bb4

    BB [3/4]
        v7. BoundsCheck i64 v0, v100 -- dominated by v4
        v8. Add   i64 v5, v0
        v9. ZeroCheck i64 v8
        v10. Div v0, v8

    BB [4/4]
        v11. ZeroCheck i64 v8 -- NOT dominated by v9
        v12. Div v0, v8
        v13. ZeroCheck i64 v0 -- dominated by v2
        v14. Mod i64 v12, v0
        v15. Ret i64 v14
    end
    */
    auto graph = std::make_shared<Graph>("checks_elimination_test2");

    auto* bb1 = new BasicBlock{1, graph};
    auto* bb2 = new BasicBlock{2, graph};
    auto* bb3 = new BasicBlock{3, graph};
    auto* bb4 = new BasicBlock{4, graph};

    graph->insertBB(bb1);
    graph->insertBB(bb2);
    graph->insertBB(bb3);
    graph->insertBB(bb4);
    graph->addEdge(bb2, bb4);

    auto* v0 = new ParamInst{0, DataType::i64, "a0"};
    auto* v1 = new ConstInst{1, static_cast<uint64_t>(0)};
    auto* v100 = new ConstInst{100, std::numeric_limits<uint64_t>::max()};
    bb1->pushBackInst(v0);
    bb1->pushBackInst(v1);
    bb1->pushBackInst(v100);

    auto* v2 = new UnaryInst{2, InstType::ZeroCheck, v0};
    auto* v3 = new BinaryInst{3, InstType::Div, v1, v0};
    auto* v4 = new BinaryInst{4, InstType::BoundsCheck, v0, v100};
    auto* v5 = new BinaryInst{5, InstType::Mul, v3, v0};
    auto* v55 = new BinaryInst{55, InstType::Cmp, v5, v0};
    auto* v6 = new JumpInst{6, InstType::Ja, bb4};
    bb2->pushBackInst(v2);
    bb2->pushBackInst(v3);
    bb2->pushBackInst(v4);
    bb2->pushBackInst(v5);
    bb2->pushBackInst(v55);
    bb2->pushBackInst(v6);

    auto* v7 = new BinaryInst{7, InstType::BoundsCheck, v0, v100};
    auto* v8 = new BinaryInst{8, InstType::Add, v5, v0};
    auto* v9 = new UnaryInst{9, InstType::ZeroCheck, v8};
    auto* v10 = new BinaryInst{10, InstType::Div, v0, v8};
    bb3->pushBackInst(v7);
    bb3->pushBackInst(v8);
    bb3->pushBackInst(v9);
    bb3->pushBackInst(v10);

    auto* v11 = new UnaryInst{11, InstType::ZeroCheck, v8};
    auto* v12 = new BinaryInst{12, InstType::Div, v0, v8};
    auto* v13 = new UnaryInst{13, InstType::ZeroCheck, v0};
    auto* v14 = new BinaryInst{14, InstType::Mod, v12, v0};
    auto* v15 = new UnaryInst{15, InstType::Return, v14};
    bb4->pushBackInst(v11);
    bb4->pushBackInst(v12);
    bb4->pushBackInst(v13);
    bb4->pushBackInst(v14);
    bb4->pushBackInst(v15);

    // graph->dump();
    graph->runPass<ChecksElimination>();
    // graph->dump();
    ASSERT_EQ(bb3->getFirstInst(), v8);
    ASSERT_EQ(v8->getPrev(), nullptr);
    ASSERT_EQ(bb4->getFirstInst(), v11);
    ASSERT_EQ(v12->getPrev(), v11);
    ASSERT_EQ(v12->getNext(), v14);
    ASSERT_EQ(v14->getPrev(), v12);
}
