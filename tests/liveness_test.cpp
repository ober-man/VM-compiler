#include "ir/graph.h"
#include "pass/liveness.h"
#include "gtest/gtest.h"

using namespace compiler;

void checkLiveIntervals(std::shared_ptr<Graph> g, 
                        std::unordered_map<uint32_t, std::pair<uint32_t, uint32_t>> expected)
{
    auto& live_intervals = g->getLiveIntervals();
    for (auto [inst, interval] : live_intervals)
    {
        uint32_t id = inst->getId();
        auto elem = expected[id];
        ASSERT_EQ(interval->getIntervalStart(), expected[id].first);
        ASSERT_EQ(interval->getIntervalEnd(), expected[id].second);
        /*
        std::cout << "Inst " << inst->getLiveNum() << "(" << inst->getLinearNum() << ") ";
        std::cout << inst->getId() << " ";
        std::cout << interval->getIntervalStart() << " " << interval->getIntervalEnd() << std::endl;
        */
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
 * 
 * 
 */
TEST(LIVENESS_TEST, TEST1)
{
    /*
    Graph for proc (preds, succs omitted)
    BB [1/4] (live 0)
        v0. Param i32 a0 (live 2)
        v1. Const i64 0 (live 4)

    BB [2/4] (live 6)
        v2. Mov   i64 r0, v0 (live 8)
        v3. Mov   i64 r1, v1 (live 10)
        v4. Cmp   i64 v2, v3 (live 12)
        v5. Ja    bb4 (live 14)

    BB [3/4] (live 16)
        v6. Add   i64 v4, v2 (live 18)
        v7. Jmp   bb4 (live 20)

    BB [4/4] (live 22)
        v8. Phi   (v3, bb2) (v6, bb3) (live 22)
        v9. Ret   i64 v8 (live 24)
    */
    auto graph = std::make_shared<Graph>("liveness_test1");

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

    auto* v2 = new MovInst{2, 0, v0};
    auto* v3 = new MovInst{3, 1, v1};
    auto* v4 = new BinaryInst{4, BinOpType::Cmp, v2, v3};
    auto* v5 = new JumpInst{5, JumpOpType::Ja, bb4};
    bb2->pushBackInst(v2);
    bb2->pushBackInst(v3);
    bb2->pushBackInst(v4);
    bb2->pushBackInst(v5);

    auto* v6 = new BinaryInst{6, BinOpType::Add, v4, v2};
    auto* v7 = new JumpInst{7, JumpOpType::Jmp, bb4};
    bb3->pushBackInst(v6);
    bb3->pushBackInst(v7);

    auto* v8 = new PhiInst{8};
    v8->addInput(std::make_pair(v3, bb2));
    v8->addInput(std::make_pair(v6, bb3));
    auto* v9 = new UnaryInst{9, UnOpType::Return, v8};
    bb4->pushBackPhiInst(v8);
    bb4->pushBackInst(v9);

    graph->runPassLivenessAnalysis();
    checkLiveIntervals(graph, { {0, {2, 8}}, {1, {4, 10}}, {2, {8, 18}}, 
                                {3, {10, 16}}, {4, {12, 18}}, {5, {14, 16}},
                                {6, {18, 22}}, {7, {20, 22}}, {8, {22, 24}}, 
                                {9, {24, 26}} });
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
TEST(LIVENESS_TEST, TEST2)
{
    /*
    Graph for proc (preds, succs omitted)
    BB [1/6] (live 0)
        v0.  Param i64 a0 (live 2)
        v1.  Const i64 0 (live 4)
        v2.  Const i64 10 (live 6)

    BB [2/6] (live 8)
        v3.  Phi   (v1, bb1) (v15, bb6) (live 8)
        v4.  Mov   i64 r0, v0 (live 10)
        v5.  Mov   i64 r1, v1 (live 12)
        v6.  Sub   i64 v3, v2 (live 14)
        v7.  Jmp   bb3 (live 16)

    BB [3/6] (live 18)
        v8.  Add   i64 v4, v1 (live 20)
        v9.  Mul   i64 v8, v5 (live 22)
        v10. Jae   bb5 (live 24)

    BB [4/6] (live 26)
        v11. Div   i64 v9, v2 (live 28)
        v12. Jb    bb5 (live 30)

    BB [5/6] (live 36)
        v13. Phi   (v8, bb3) (v11, bb4) (live 36)
        v14. Sub   i64 v13, v0 (live 38)
        v15. Ret   i64 v14 (live 40)

    BB [6/6] (live 32)
        v16. Sub   i64 v11, v5 (live 36)
    */
    auto graph = std::make_shared<Graph>("liveness_test2");

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
    auto* v4 = new MovInst{4, 0, v0};
    auto* v5 = new MovInst{5, 1, v1};
    auto* v6 = new BinaryInst{6, BinOpType::Sub, v3, v2};
    auto* v7 = new JumpInst{7, JumpOpType::Jmp, bb3};
    bb2->pushBackPhiInst(v3);
    bb2->pushBackInst(v4);
    bb2->pushBackInst(v5);
    bb2->pushBackInst(v6);
    bb2->pushBackInst(v7);

    auto* v8 = new BinaryInst{8, BinOpType::Add, v4, v1};
    auto* v9 = new BinaryInst{9, BinOpType::Mul, v8, v5};
    auto* v10 = new JumpInst{10, JumpOpType::Jae, bb5};
    bb3->pushBackInst(v8);
    bb3->pushBackInst(v9);
    bb3->pushBackInst(v10);

    auto* v11 = new BinaryInst{11, BinOpType::Div, v9, v2};
    auto* v12 = new JumpInst{12, JumpOpType::Jb, bb5};
    bb4->pushBackInst(v11);
    bb4->pushBackInst(v12);

    auto* v13 = new PhiInst{13};
    v13->addInput(std::make_pair(v8, bb3));
    v13->addInput(std::make_pair(v11, bb4));
    auto* v14 = new BinaryInst{14, BinOpType::Sub, v13, v0};
    auto* v15 = new UnaryInst{15, UnOpType::Return, v14};
    bb5->pushBackPhiInst(v13);
    bb5->pushBackInst(v14);
    bb5->pushBackInst(v15);

    auto* v16 = new BinaryInst{16, BinOpType::Sub, v11, v5};
    bb6->pushBackInst(v16);

    v3->addInput(std::make_pair(v1, bb1));
    v3->addInput(std::make_pair(v15, bb6));
    // graph->dump();

    graph->runPassLivenessAnalysis();
    checkLiveIntervals(graph, { {0, {2, 38}}, {1, {4, 20}}, {2, {6, 28}}, 
                                {3, {8, 14}}, {4, {10, 20}}, {5, {12, 34}},
                                {6, {14, 16}}, {7, {16, 18}}, {8, {20, 26}}, 
                                {9, {22, 28}}, {10, {24, 26}}, {11, {28, 34}},
                                {12, {30, 32}}, {13, {36, 38}}, {14, {38, 40}},
                                {15, {40, 42}}, {16, {34, 36}}});
}

TEST(LIVENESS_TEST, FACT)
{
    /*
    Graph for proc fact (preds, succs omitted)
    BB [1/5] (live 0)
        v0. Param i32 a0 (live 2)
        v1. Const i64 1 (live 4)
        v2. Const i64 2 (live 6)

    BB [2/5] (live 8)
        v3. Mov   i64 r0, v1 (live 10)
        v4. Mov   i64 r1, v2 (live 12)
        v5. Cast  v0 to i64 (live 14)

    BB [3/5] (live 16)
        v6. Phi   (v4, bb2) (v12, bb4) (live 16)
        v7. Cmp   i64 v6, v5 (live 18)
        v8. Ja    bb5 (live 20)

    BB [4/5] (live 22)
        v9.  Phi  (v3, bb2) (v11, bb4) (live 22)
        v10. Phi  (v4, bb2) (v12, bb4) (live 22)
        v11. Mul  i64 v9, v10 (live 24)
        v12. Add  i64 v10, v1 (live 26)
        v13. Jmp  bb3 (live 28)

    BB [5/5] (live 30)
        v14. Phi  (v3, bb2) (v11, bb4) (live 30)
        v15. Ret  i64 v14 (live 32)
    */

    auto graph = std::make_shared<Graph>("fact");

    auto* bb1 = new BasicBlock{1, graph};
    auto* bb2 = new BasicBlock{2, graph};
    auto* bb3 = new BasicBlock{3, graph};
    auto* bb4 = new BasicBlock{4, graph};
    auto* bb5 = new BasicBlock{5, graph};

    // Fill bb1
    auto* v0 = new ParamInst{0, DataType::i32, "a0"};
    auto* v1 = new ConstInst{1, static_cast<uint64_t>(1)};
    auto* v2 = new ConstInst{2, static_cast<uint64_t>(2)};

    bb1->pushBackInst(v0);
    bb1->pushBackInst(v1);
    bb1->pushBackInst(v2);

    // Fill bb2
    auto* v3 = new MovInst{3, 0, v1};
    auto* v4 = new MovInst{4, 1, v2};
    auto* v5 = new CastInst{5, v0, DataType::i64};

    bb2->pushBackInst(v3);
    bb2->pushBackInst(v4);
    bb2->pushBackInst(v5);

    // Fill bb3
    auto* v6 = new PhiInst{6};
    v6->addInput(std::make_pair(v4, bb2));
    auto* v7 = new BinaryInst{7, BinOpType::Cmp, v6, v5};
    auto* v8 = new JumpInst{8, JumpOpType::Ja, bb5};

    bb3->pushBackPhiInst(v6);
    bb3->pushBackInst(v7);
    bb3->pushBackInst(v8);

    // Fill bb4
    auto* v9 = new PhiInst{9};
    v9->addInput(std::make_pair(v3, bb2));
    auto* v10 = new PhiInst{10};
    v10->addInput(std::make_pair(v4, bb2));
    auto* v11 = new BinaryInst{11, BinOpType::Mul, v9, v10};
    auto* v12 = new BinaryInst{12, BinOpType::Add, v10, v1};
    auto* v13 = new JumpInst{13, JumpOpType::Jmp, bb3};

    v6->addInput(std::make_pair(v12, bb4));
    v9->addInput(std::make_pair(v11, bb4));
    v10->addInput(std::make_pair(v12, bb4));

    bb4->pushBackPhiInst(v9);
    bb4->pushBackPhiInst(v10);
    bb4->pushBackInst(v11);
    bb4->pushBackInst(v12);
    bb4->pushBackInst(v13);

    // Fill bb5
    auto* v14 = new PhiInst{14};
    v14->addInput(std::make_pair(v3, bb2));
    v14->addInput(std::make_pair(v11, bb4));
    auto* v15 = new UnaryInst{15, UnOpType::Return, v14};

    bb5->pushBackPhiInst(v14);
    bb5->pushBackInst(v15);

    graph->insertBB(bb1);
    graph->insertBB(bb2);
    graph->insertBB(bb3);
    graph->insertBB(bb4);
    graph->insertBBAfter(bb3, bb5, false);
    graph->addEdge(bb4, bb3);
    // graph->dump();

    graph->runPassLivenessAnalysis();
    checkLiveIntervals(graph, { {0, {2, 14}}, {1, {4, 26}}, {2, {6, 12}}, 
                                {3, {10, 12}}, {4, {12, 16}}, {5, {14, 22}},
                                {6, {16, 18}}, {7, {18, 20}}, {8, {20, 22}}, 
                                {9, {22, 24}}, {10, {22, 26}}, {11, {24, 26}},
                                {12, {26, 28}}, {13, {28, 30}}, {14, {30, 32}},
                                {15, {32, 34}}, {16, {34, 36}}});
}