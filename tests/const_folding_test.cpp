#include "ir/graph.h"
#include "pass/const_folding.h"
#include "pass/dce.h"
#include "gtest/gtest.h"

using namespace compiler;

/**
 * Test1 graph:
 *                 [1]
 *                  |
 *                  v
 *             /---[2]---\
 *             |         |
 *             v         v
 *            [3]------>[4]
 */
TEST(CONST_FOLDING_TEST, TEST1)
{
    /*
    Graph for proc (preds, succs omitted)
    BB [1/4]
        v0. Param i32 a0
        v1. Const i64 0
        v100. Const i64 1
        v200. Const i64 2
        v300. Const i64 5
        v400. Const i64 10

    BB [2/4]
        v15. Mul  i64 v200, v300
        v2. Cmp   i64 v0, v15
        v3. Ja    bb4

    BB [3/4]
        v35. Add  i64 v100, v200
        v4. Add   i64 v35, v0
        v5. Jmp   bb4

    BB [4/4]
        v6. Phi   (v1, bb2) (v4, bb3)
        v7. Ret   i64 v6
    end
    */
    auto graph = std::make_shared<Graph>("const_folding_test1");

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
    auto* v100 = new ConstInst{100, static_cast<uint64_t>(1)};
    auto* v200 = new ConstInst{200, static_cast<uint64_t>(2)};
    auto* v300 = new ConstInst{300, static_cast<uint64_t>(5)};
    auto* v400 = new ConstInst{400, static_cast<uint64_t>(10)};
    bb1->pushBackInst(v0);
    bb1->pushBackInst(v1);
    bb1->pushBackInst(v100);
    bb1->pushBackInst(v200);
    bb1->pushBackInst(v300);
    bb1->pushBackInst(v400);

    auto* v15 = new BinaryInst{15, InstType::Mul, v200, v300};
    auto* v2 = new BinaryInst{2, InstType::Cmp, v0, v15};
    auto* v3 = new JumpInst{3, InstType::Ja, bb4};
    bb2->pushBackInst(v15);
    bb2->pushBackInst(v2);
    bb2->pushBackInst(v3);

    auto* v35 = new BinaryInst{35, InstType::Add, v100, v200};
    auto* v4 = new BinaryInst{4, InstType::Add, v35, v0};
    auto* v5 = new JumpInst{5, InstType::Jmp, bb4};
    bb3->pushBackInst(v35);
    bb3->pushBackInst(v4);
    bb3->pushBackInst(v5);

    auto* v6 = new PhiInst{6};
    v6->addInput(std::make_pair(v1, bb2));
    v6->addInput(std::make_pair(v4, bb3));
    auto* v7 = new UnaryInst{7, InstType::Return, v6};
    bb4->pushBackPhiInst(v6);
    bb4->pushBackInst(v7);

    // graph->dump();
    graph->runPass<ConstFolding>();
    graph->runPass<Dce>();
    // graph->dump();
    ASSERT_EQ(static_cast<BinaryInst*>(v2)->getInput(1), v400);
    ASSERT_EQ(static_cast<BinaryInst*>(v4)->getInput(0), bb1->getLastInst());
    ASSERT_EQ(static_cast<ConstInst*>(bb1->getLastInst())->getIntValue(), 3);
}

/**
 * Test2 graph:
 *                  [1]
 *                   |
 *                   v
 *             /----[2]<-----\
 *             |             |
 *             v             |
 *            [3]-->[4]---->[6]
 *             |     |
 *             |     v
 *             \--->[5]
 */
TEST(CONST_FOLDING_TEST, TEST2)
{
    /*
    Graph for proc (preds, succs omitted)
    BB [1/6]
        v0.  Param i64 a0
        v1.  Const i64 0
        v2.  Const i64 10
        v100. Const i64 1
        v200. Const i64 2

    BB [2/6]
        v3.  Phi   (v1, bb1) (v14, bb6)
        v4.  Sub   i64 v3, v2
        v5.  Jmp   bb3

    BB [3/6]
        v6.  Or    i64 v100, v2
        v7.  Mul   i64 v6, v1
        v75. Cmp   i64 v7, v2
        v8.  Jae   bb5

    BB [4/6]
        v9.  AShr  i64 v2, v200
        v95. Cmp   i64 v9, v1
        v10. Jb    bb5

    BB [5/6]
        v11. Phi   (v6, bb3) (v9, bb4)
        v12. Sub   i64 v11, v0
        v13. Ret   i64 v12
    end

    BB [6/6]
        v14. Sub   i64 v2, v100
        v15. Jmp   bb2
    */
    auto graph = std::make_shared<Graph>("const_folding_test2");

    auto* bb1 = new BasicBlock{1, graph};
    auto* bb2 = new BasicBlock{2, graph};
    auto* bb3 = new BasicBlock{3, graph};
    auto* bb4 = new BasicBlock{4, graph};
    auto* bb5 = new BasicBlock{5, graph};
    auto* bb6 = new BasicBlock{6, graph};

    graph->insertBB(bb1);
    graph->insertBB(bb2);
    graph->insertBB(bb3);
    graph->insertBBAfter(bb3, bb4, true);
    graph->insertBBAfter(bb3, bb5, false);
    graph->insertBBAfter(bb4, bb6, true);
    graph->addEdge(bb4, bb5);
    graph->addEdge(bb6, bb2);

    auto* v0 = new ParamInst{0, DataType::i64, "a0"};
    auto* v1 = new ConstInst{1, static_cast<uint64_t>(0)};
    auto* v2 = new ConstInst{2, static_cast<uint64_t>(10)};
    auto* v100 = new ConstInst{100, static_cast<uint64_t>(1)};
    auto* v200 = new ConstInst{200, static_cast<uint64_t>(2)};
    bb1->pushBackInst(v0);
    bb1->pushBackInst(v1);
    bb1->pushBackInst(v2);
    bb1->pushBackInst(v100);
    bb1->pushBackInst(v200);

    auto* v3 = new PhiInst{3};
    auto* v4 = new BinaryInst{4, InstType::Sub, v3, v2};
    auto* v5 = new JumpInst{5, InstType::Jmp, bb3};
    bb2->pushBackPhiInst(v3);
    bb2->pushBackInst(v4);
    bb2->pushBackInst(v5);

    auto* v6 = new BinaryInst{6, InstType::Or, v100, v2};
    auto* v7 = new BinaryInst{7, InstType::Mul, v6, v1};
    auto* v75 = new BinaryInst{75, InstType::Cmp, v7, v2};
    auto* v8 = new JumpInst{8, InstType::Jae, bb5};
    bb3->pushBackInst(v6);
    bb3->pushBackInst(v7);
    bb3->pushBackInst(v75);
    bb3->pushBackInst(v8);

    auto* v9 = new BinaryInst{9, InstType::AShr, v2, v200};
    auto* v95 = new BinaryInst{95, InstType::Cmp, v9, v1};
    auto* v10 = new JumpInst{10, InstType::Jb, bb5};
    bb4->pushBackInst(v9);
    bb4->pushBackInst(v95);
    bb4->pushBackInst(v10);

    auto* v11 = new PhiInst{11};
    v11->addInput(std::make_pair(v6, bb3));
    v11->addInput(std::make_pair(v9, bb4));
    auto* v12 = new BinaryInst{12, InstType::Sub, v11, v0};
    auto* v13 = new UnaryInst{13, InstType::Return, v12};
    bb5->pushBackPhiInst(v11);
    bb5->pushBackInst(v12);
    bb5->pushBackInst(v13);

    auto* v14 = new BinaryInst{14, InstType::Sub, v2, v100};
    auto* v15 = new JumpInst{15, InstType::Jmp, bb2};
    bb6->pushBackInst(v14);
    bb6->pushBackInst(v15);

    v3->addInput(std::make_pair(v1, bb1));
    v3->addInput(std::make_pair(v14, bb6));

    // graph->dump();
    graph->runPass<ConstFolding>();
    graph->runPass<Dce>();
    // graph->dump();

    ASSERT_EQ(bb2->getFirstInst(), v5);
    ASSERT_EQ(static_cast<PhiInst*>(v3)->getInputs()[1].first, bb1->getLastInst());
    ASSERT_EQ(static_cast<ConstInst*>(bb1->getLastInst())->getIntValue(), 9);
    ASSERT_EQ(static_cast<ConstInst*>(bb1->getLastInst()->getPrev())->getIntValue(), 11);
    ASSERT_EQ(bb3->getFirstInst(), v75);
    ASSERT_EQ(static_cast<BinaryInst*>(v75)->getInput(0), v1);
    ASSERT_EQ(static_cast<BinaryInst*>(v95)->getInput(0), v200);
    ASSERT_EQ(static_cast<PhiInst*>(v11)->getInputs()[0].first, bb1->getLastInst()->getPrev());
    ASSERT_EQ(static_cast<PhiInst*>(v11)->getInputs()[1].first, v200);
}