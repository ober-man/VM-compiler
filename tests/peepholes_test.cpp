#include "ir/graph.h"
#include "pass/dce.h"
#include "pass/peepholes.h"
#include "gtest/gtest.h"

using namespace compiler;

TEST(PEEPHOLES_TEST, TEST_MUL)
{
    /*
    Graph for proc (preds, succs omitted)
    BB [1/5]
        v0. Param i32 a0
        v1. Const i64 0
        v100. Const i64 1
        v200. Const i64 -1
        v300. Const i64 64

    BB [2/5]
        v15. Mul  i64 v0, v1
        v2. Sub   i64 v0, v15
        v3. Jmp   bb3

    BB [3/5]
        v35. Mul  i64 v2, v100
        v4. Add   i64 v35, v0
        v5. Jmp   bb4

    BB [4/5]
        v6. Mul   i64 v4, v200
        v7. Div   i64 v35, v0
        v8. Jmp   bb5

    BB [5/5]
        v9.  Mul   i64 v6, v300
        v10. Ret   i64 v9
    end
    */
    auto graph = std::make_shared<Graph>("peepholes_test_mul");

    auto* bb1 = new BasicBlock{1, graph};
    auto* bb2 = new BasicBlock{2, graph};
    auto* bb3 = new BasicBlock{3, graph};
    auto* bb4 = new BasicBlock{4, graph};
    auto* bb5 = new BasicBlock{5, graph};

    graph->insertBB(bb1);
    graph->insertBB(bb2);
    graph->insertBB(bb3);
    graph->insertBB(bb4);
    graph->insertBB(bb5);

    auto* v0 = new ParamInst{0, DataType::i32, "a0"};
    auto* v1 = new ConstInst{1, static_cast<uint64_t>(0)};
    auto* v100 = new ConstInst{100, static_cast<uint64_t>(1)};
    auto* v200 = new ConstInst{200, static_cast<uint64_t>(-1)};
    auto* v300 = new ConstInst{300, static_cast<uint64_t>(64)};
    bb1->pushBackInst(v0);
    bb1->pushBackInst(v1);
    bb1->pushBackInst(v100);
    bb1->pushBackInst(v200);
    bb1->pushBackInst(v300);

    auto* v15 = new BinaryInst{15, InstType::Mul, v0, v1};
    auto* v2 = new BinaryInst{2, InstType::Sub, v0, v15};
    auto* v3 = new JumpInst{3, InstType::Jmp, bb3};
    bb2->pushBackInst(v15);
    bb2->pushBackInst(v2);
    bb2->pushBackInst(v3);

    auto* v35 = new BinaryInst{35, InstType::Mul, v2, v100};
    auto* v4 = new BinaryInst{4, InstType::Add, v35, v0};
    auto* v5 = new JumpInst{5, InstType::Jmp, bb4};
    bb3->pushBackInst(v35);
    bb3->pushBackInst(v4);
    bb3->pushBackInst(v5);

    auto* v6 = new BinaryInst{6, InstType::Mul, v4, v200};
    auto* v7 = new BinaryInst{7, InstType::Div, v35, v0};
    auto* v8 = new JumpInst{8, InstType::Jmp, bb5};
    bb4->pushBackInst(v6);
    bb4->pushBackInst(v7);
    bb4->pushBackInst(v8);

    auto* v9 = new BinaryInst{35, InstType::Mul, v6, v300};
    auto* v10 = new UnaryInst{13, InstType::Return, v9};
    bb5->pushBackInst(v9);
    bb5->pushBackInst(v10);

    // graph->dump();
    graph->runPass<Peepholes>();
    graph->runPass<Dce>();
    // graph->dump();
    ASSERT_EQ(static_cast<BinaryInst*>(v2)->getInput(1), v1);
    ASSERT_EQ(static_cast<BinaryInst*>(v4)->getInput(1), v0);
    ASSERT_EQ(bb4->getFirstInst()->getInstType(), InstType::Neg);
    ASSERT_EQ(bb5->getFirstInst()->getInstType(), InstType::Shl);
    ASSERT_EQ(static_cast<ConstInst*>(bb1->getLastInst())->getIntValue(), 6);
}

TEST(PEEPHOLES_TEST, TEST_OR)
{
    /*
    Graph for proc (preds, succs omitted)
    BB [1/5]
        v0. Param i32 a0
        v1. Const i64 0
        v100. Const i64 111...111

    BB [2/5]
        v15. Or   i64 v0, v0
        v2. Sub   i64 v0, v15
        v3. Jmp   bb3

    BB [3/5]
        v35. Or   i64 v2, v1
        v4. Add   i64 v35, v0
        v5. Jmp   bb4

    BB [4/5]
        v6. Or    i64 v4, v100
        v7. Div   i64 v6, v0
        v8. Jmp   bb5

    BB [5/5]
        v9.  Not   i64 v4
        v10. Not   i64 v7
        v11. Or    i64 v9, v10
        v12. Ret   i64 v11
    end
    */
    auto graph = std::make_shared<Graph>("peepholes_test_or");

    auto* bb1 = new BasicBlock{1, graph};
    auto* bb2 = new BasicBlock{2, graph};
    auto* bb3 = new BasicBlock{3, graph};
    auto* bb4 = new BasicBlock{4, graph};
    auto* bb5 = new BasicBlock{5, graph};

    graph->insertBB(bb1);
    graph->insertBB(bb2);
    graph->insertBB(bb3);
    graph->insertBB(bb4);
    graph->insertBB(bb5);

    auto* v0 = new ParamInst{0, DataType::i32, "a0"};
    auto* v1 = new ConstInst{1, static_cast<uint64_t>(0)};
    auto* v100 = new ConstInst{100, std::numeric_limits<uint64_t>::max()};
    bb1->pushBackInst(v0);
    bb1->pushBackInst(v1);
    bb1->pushBackInst(v100);

    auto* v15 = new BinaryInst{15, InstType::Or, v0, v0};
    auto* v2 = new BinaryInst{2, InstType::Sub, v0, v15};
    auto* v3 = new JumpInst{3, InstType::Jmp, bb3};
    bb2->pushBackInst(v15);
    bb2->pushBackInst(v2);
    bb2->pushBackInst(v3);

    auto* v35 = new BinaryInst{35, InstType::Or, v2, v1};
    auto* v4 = new BinaryInst{4, InstType::Add, v35, v0};
    auto* v5 = new JumpInst{5, InstType::Jmp, bb4};
    bb3->pushBackInst(v35);
    bb3->pushBackInst(v4);
    bb3->pushBackInst(v5);

    auto* v6 = new BinaryInst{6, InstType::Or, v4, v100};
    auto* v7 = new BinaryInst{7, InstType::Div, v6, v0};
    auto* v8 = new JumpInst{8, InstType::Jmp, bb5};
    bb4->pushBackInst(v6);
    bb4->pushBackInst(v7);
    bb4->pushBackInst(v8);

    auto* v9 = new UnaryInst{9, InstType::Not, v4};
    auto* v10 = new UnaryInst{10, InstType::Not, v7};
    auto* v11 = new BinaryInst{11, InstType::Or, v9, v10};
    auto* v12 = new UnaryInst{12, InstType::Return, v11};
    bb5->pushBackInst(v9);
    bb5->pushBackInst(v10);
    bb5->pushBackInst(v11);
    bb5->pushBackInst(v12);

    // graph->dump();
    graph->runPass<Peepholes>();
    graph->runPass<Dce>();
    // graph->dump();
    ASSERT_EQ(static_cast<BinaryInst*>(v2)->getInput(1), v0);
    ASSERT_EQ(static_cast<BinaryInst*>(v4)->getInput(0), v2);
    ASSERT_EQ(static_cast<BinaryInst*>(v7)->getInput(0), bb1->getLastInst());
    ASSERT_EQ(static_cast<ConstInst*>(bb1->getLastInst())->getIntValue(), 1);
    ASSERT_EQ(bb5->getFirstInst()->getInstType(), InstType::And);
}

TEST(PEEPHOLES_TEST, TEST_ASHR)
{
    /*
    Graph for proc (preds, succs omitted)
    BB [1/3]
        v0. Param i32 a0
        v1. Const i64 0
        v100. Const i64 2

    BB [2/3]
        v15. AShr  i64 v0, v1
        v2. Sub    i64 v0, v15
        v3. Ret   v2
    end
    */
    auto graph = std::make_shared<Graph>("peepholes_test_ashr");

    auto* bb1 = new BasicBlock{1, graph};
    auto* bb2 = new BasicBlock{2, graph};

    graph->insertBB(bb1);
    graph->insertBB(bb2);

    auto* v0 = new ParamInst{0, DataType::i32, "a0"};
    auto* v1 = new ConstInst{1, static_cast<uint64_t>(0)};
    auto* v100 = new ConstInst{100, static_cast<uint64_t>(2)};
    bb1->pushBackInst(v0);
    bb1->pushBackInst(v1);
    bb1->pushBackInst(v100);

    auto* v15 = new BinaryInst{15, InstType::AShr, v0, v1};
    auto* v2 = new BinaryInst{2, InstType::Sub, v0, v15};
    auto* v3 = new UnaryInst{6, InstType::Return, v2};
    bb2->pushBackInst(v15);
    bb2->pushBackInst(v2);
    bb2->pushBackInst(v3);

    // graph->dump();
    graph->runPass<Peepholes>();
    graph->runPass<Dce>();
    // graph->dump();
    ASSERT_EQ(static_cast<BinaryInst*>(v2)->getInput(1), v0);
}