#include "graph.h"
#include "gtest/gtest.h"

using namespace compiler;

TEST(IR_TEST, FACT)
{
    /*
    Graph for proc fact (preds, succs omitted)
    BB [1/5]
        v1. param i32 a0
        v2. const i64 1
        v3. const i64 2

    BB [2/5]
        v4. mov   i64 r0, v2
        v5. mov   i64 r1, v3
        v6. cast  v1 to i64

    BB [3/5]
        v7. phi   (v5, bb2) (v13, bb4)
        v8. cmp   i64 v7, v6
        v9. ja    bb5

    BB [4/5]
        v10. phi  (v4, bb2) (v12, bb4)
        v11. phi  (v5, bb2) (v13, bb4)
        v12. mul  i64 v10, v11
        v13. add  i64 v11, v2
        v14. jmp  bb3

    BB [5/5]
        v15. phi  (v4, bb2) (v12, bb4)
        v16. ret  i64 v15
    */

    auto graph = std::make_shared<Graph>(Graph{"fact"});

    auto bb1 = std::make_shared<BasicBlock>(BasicBlock{1, graph});
    auto bb2 = std::make_shared<BasicBlock>(BasicBlock{2, graph});
    auto bb3 = std::make_shared<BasicBlock>(BasicBlock{3, graph});
    auto bb4 = std::make_shared<BasicBlock>(BasicBlock{4, graph});
    auto bb5 = std::make_shared<BasicBlock>(BasicBlock{5, graph});

    // Fill bb1
    auto v1 = std::make_shared<ParamInst>(ParamInst{1, DataType::Int32, "a0"});
    auto v2 = std::make_shared<ConstInst>(ConstInst{2, 1});
    auto v3 = std::make_shared<ConstInst>(ConstInst{3, 2});

    bb1->pushBackInst(v1);
    bb1->pushBackInst(v2);
    bb1->pushBackInst(v3);

    // Fill bb2
    auto v4 = std::make_shared<MovInst>(MovInst{4, 0, v2});
    auto v5 = std::make_shared<MovInst>(MovInst{5, 1, v3});
    auto v6 = std::make_shared<CastInst>(CastInst{6, v1, DataType::Int64});

    bb2->pushBackInst(v4);
    bb2->pushBackInst(v5);
    bb2->pushBackInst(v6);

    // Fill bb3
    auto v7 = std::make_shared<PhiInst>(PhiInst{7});
    v7->addInput(std::make_pair(v5, bb2));
    auto v8 =
        std::make_shared<BinaryInst>(BinaryInst{8, BinOpType::Cmp, v7, v6});
    auto v9 = std::make_shared<JumpInst>(JumpInst{9, JumpOpType::Ja, bb5});

    bb3->pushBackInst(v7);
    bb3->pushBackInst(v8);
    bb3->pushBackInst(v9);

    // Fill bb4
    auto v10 = std::make_shared<PhiInst>(PhiInst{10});
    v10->addInput(std::make_pair(v4, bb2));
    auto v11 = std::make_shared<PhiInst>(PhiInst{11});
    v11->addInput(std::make_pair(v5, bb2));
    auto v12 =
        std::make_shared<BinaryInst>(BinaryInst{12, BinOpType::Mul, v10, v11});
    auto v13 =
        std::make_shared<BinaryInst>(BinaryInst{13, BinOpType::Add, v11, v2});
    auto v14 = std::make_shared<JumpInst>(JumpInst{14, JumpOpType::Jmp, bb3});

    v7->addInput(std::make_pair(v13, bb4));
    v10->addInput(std::make_pair(v12, bb4));
    v11->addInput(std::make_pair(v13, bb4));

    bb4->pushBackInst(v10);
    bb4->pushBackInst(v11);
    bb4->pushBackInst(v12);
    bb4->pushBackInst(v13);
    bb4->pushBackInst(v14);

    // Fill bb5
    auto v15 = std::make_shared<PhiInst>(PhiInst{15});
    v15->addInput(std::make_pair(v4, bb2));
    v15->addInput(std::make_pair(v12, bb4));
    auto v16 =
        std::make_shared<UnaryInst>(UnaryInst{16, UnOpType::Return, v15});

    bb5->pushBackInst(v15);
    bb5->pushBackInst(v16);

    graph->insertBB(bb1);
    graph->insertBB(bb2);
    graph->insertBB(bb3);
    graph->insertBB(bb4);
    graph->insertBB(bb5);

    graph->dump();
    ASSERT_EQ(2 * 2, 4);
}
