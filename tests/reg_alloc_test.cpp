#include "ir/graph.h"
#include "pass/reg_alloc.h"
#include "gtest/gtest.h"

using namespace compiler;

void checkRegisters(std::shared_ptr<Graph> g,
                    std::unordered_map<uint32_t, std::pair<char, uint32_t>> expected)
{
    auto& live_intervals = g->getLiveIntervals();
    for (auto [inst, interval] : live_intervals)
    {
        if (inst->isJmpInst())
            continue;
        uint32_t id = inst->getId();
        auto elem = expected[id];
        ASSERT_EQ(interval->isRealRegister() ? 'r' : 's', elem.first);
        ASSERT_EQ(interval->getLocation(), elem.second);
    }
}

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
TEST(REGALLOC_TEST, TEST1)
{
    /*
    Graph for proc (preds, succs omitted)
    BB [1/4] (live 0)
        v0. Param i32 a0 (live 2)
        v1. Const i64 0 (live 4)

    BB [2/4] (live 6)
        v2. Cmp   i64 v0, v1 (live 8)
        v3. Ja    bb4 (live 10)

    BB [3/4] (live 12)
        v4. Add   i64 v1, v0 (live 14)
        v5. Jmp   bb4 (live 16)

    BB [4/4] (live 18)
        v6. Phi   (v1, bb2) (v4, bb3) (live 18)
        v7. Ret   i64 v6 (live 20)
    end (live 22)
    */
    auto graph = std::make_shared<Graph>("regalloc_test1");

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

    auto* v6 = new PhiInst{6};
    v6->addInput(std::make_pair(v1, bb2));
    v6->addInput(std::make_pair(v4, bb3));
    auto* v7 = new UnaryInst{7, InstType::Return, v6};
    bb4->pushBackPhiInst(v6);
    bb4->pushBackInst(v7);

    graph->runPass<RegisterAllocation>();
    // clang-format off
    checkRegisters(graph, {{0, {'s', 0}},
                           {1, {'r', 1}},
                           {2, {'r', 0}},
                           {4, {'r', 0}},
                           {6, {'r', 0}},
                           {7, {'r', 0}}});
    /* {{0, {2, 14}},
        {1, {4, 14}},
        {2, {8, 10}},
        {3, {0, 0}},
        {4, {14, 18}},
        {5, {0, 0}},
        {6, {18, 20}},
        {7, {20, 22}}}); */
    // clang-format on
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
TEST(REGALLOC_TEST, TEST2)
{
    /*
    Graph for proc (preds, succs omitted)
    BB [1/6] (live 0)
        v0.  Param i64 a0 (live 2)
        v1.  Const i64 0 (live 4)
        v2.  Const i64 10 (live 6)

    BB [2/6] (live 8)
        v3.  Phi   (v1, bb1) (v14, bb6) (live 8)
        v4.  Sub   i64 v3, v2 (live 10)
        v5.  Jmp   bb3 (live 12)

    BB [3/6] (live 14)
        v6.  Add   i64 v0, v1 (live 16)
        v7.  Mul   i64 v6, v1 (live 18)
        v75. Cmp   i64 v7, v2 (live 20)
        v8.  Jae   bb5 (live 22)

    BB [4/6] (live 24)
        v9.  Div   i64 v7, v2 (live 26)
        v95. Cmp   i64 v9, v1 (live 28)
        v10. Jb    bb5 (live 30)

    BB [5/6] (live 38)
        v11. Phi   (v6, bb3) (v9, bb4) (live 38)
        v12. Sub   i64 v11, v0 (live 40)
        v13. Ret   i64 v12 (live 42)
    end (live 44)

    BB [6/6] (live 32)
        v14. Sub   i64 v9, v1 (live 34)
        v15. Jmp   bb2 (live 36)
    */
    auto graph = std::make_shared<Graph>("regalloc_test2");

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
    bb1->pushBackInst(v0);
    bb1->pushBackInst(v1);
    bb1->pushBackInst(v2);

    auto* v3 = new PhiInst{3};
    auto* v4 = new BinaryInst{4, InstType::Sub, v3, v2};
    auto* v5 = new JumpInst{5, InstType::Jmp, bb3};
    bb2->pushBackPhiInst(v3);
    bb2->pushBackInst(v4);
    bb2->pushBackInst(v5);

    auto* v6 = new BinaryInst{6, InstType::Add, v0, v1};
    auto* v7 = new BinaryInst{7, InstType::Mul, v6, v1};
    auto* v75 = new BinaryInst{75, InstType::Cmp, v7, v2};
    auto* v8 = new JumpInst{8, InstType::Jae, bb5};
    bb3->pushBackInst(v6);
    bb3->pushBackInst(v7);
    bb3->pushBackInst(v75);
    bb3->pushBackInst(v8);

    auto* v9 = new BinaryInst{9, InstType::Div, v7, v2};
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

    auto* v14 = new BinaryInst{14, InstType::Sub, v9, v1};
    auto* v15 = new JumpInst{15, InstType::Jmp, bb2};
    bb6->pushBackInst(v14);
    bb6->pushBackInst(v15);

    v3->addInput(std::make_pair(v1, bb1));
    v3->addInput(std::make_pair(v14, bb6));
    // graph->dump();

    graph->runPass<RegisterAllocation>();
    checkRegisters(graph, {{0, {'s', 0}},
                           {1, {'s', 1}},
                           {2, {'r', 0}},
                           {3, {'r', 1}},
                           {4, {'r', 1}},
                           {6, {'s', 3}},
                           {7, {'s', 2}},
                           {75, {'r', 1}},
                           {9, {'s', 4}},
                           {95, {'r', 1}},
                           {11, {'r', 1}},
                           {12, {'r', 1}},
                           {13, {'r', 1}},
                           {14, {'r', 1}}});
    /*{{0, {2, 40}},
       {1, {4, 38}},
       {2, {6, 38}},
       {3, {8, 10}},
       {4, {10, 12}},
       {5, {0, 0}},
       {6, {16, 24}},
       {7, {18, 26}},
       {75, {20, 22}},
       {8, {0, 0}},
       {9, {26, 34}},
       {95, {28, 30}},
       {10, {0, 0}},
       {11, {38, 40}},
       {12, {40, 42}},
       {13, {42, 44}},
       {14, {34, 36}},
       {15, {0, 0}}});*/
    // more representative pictures in directory 'pictures'
}