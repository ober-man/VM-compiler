#include "graph.h"
#include "gtest/gtest.h"

using namespace compiler;

TEST(IR_TEST, FACT)
{
    /*
    Graph for proc fact (preds, succs omitted)
    BB [1/5]
        v1. Param i32 a0
        v2. Const i64 1
        v3. Const i64 2

    BB [2/5]
        v4. Mov   i64 r0, v2
        v5. Mov   i64 r1, v3
        v6. Cast  v1 to i64

    BB [3/5]
        v7. Phi   (v5, bb2) (v13, bb4)
        v8. Cmp   i64 v7, v6
        v9. Ja    bb5

    BB [4/5]
        v10. Phi  (v4, bb2) (v12, bb4)
        v11. Phi  (v5, bb2) (v13, bb4)
        v12. Mul  i64 v10, v11
        v13. Add  i64 v11, v2
        v14. Jmp  bb3

    BB [5/5]
        v15. Phi  (v4, bb2) (v12, bb4)
        v16. Ret  i64 v15
    */

    auto graph = std::make_shared<Graph>("fact");

    auto *bb1 = new BasicBlock{1, graph};
    auto *bb2 = new BasicBlock{2, graph};
    auto *bb3 = new BasicBlock{3, graph};
    auto *bb4 = new BasicBlock{4, graph};
    auto *bb5 = new BasicBlock{5, graph};

    // Fill bb1
    auto *v1 = new ParamInst{1, DataType::i32, "a0"};
    auto *v2 = new ConstInst{2, static_cast<uint64_t>(1)};
    auto *v3 = new ConstInst{3, static_cast<uint64_t>(2)};

    bb1->pushBackInst(v1);
    bb1->pushBackInst(v2);
    bb1->pushBackInst(v3);

    {
        ASSERT_EQ(v1->getId(), 1);
        ASSERT_EQ(v2->getId(), 2);
        ASSERT_EQ(v3->getId(), 3);

        ASSERT_EQ(v1->getNext()->getId(), 2);
        ASSERT_EQ(v2->getNext()->getId(), 3);
        ASSERT_EQ(v3->getNext(), nullptr);

        ASSERT_EQ(v1->getPrev(), nullptr);
        ASSERT_EQ(v2->getPrev()->getId(), 1);
        ASSERT_EQ(v3->getPrev()->getId(), 2);

        ASSERT_EQ(v3->getBB()->getId(), 1);
    }

    // Fill bb2
    auto *v4 = new MovInst{4, 0, v2};
    auto *v5 = new MovInst{5, 1, v3};
    auto *v6 = new CastInst{6, v1, DataType::i64};

    bb2->pushBackInst(v4);
    bb2->pushBackInst(v5);
    bb2->pushBackInst(v6);

    {
        ASSERT_EQ(v4->getId(), 4);
        ASSERT_EQ(v5->getId(), 5);
        ASSERT_EQ(v6->getId(), 6);

        ASSERT_EQ(v4->getNext()->getId(), 5);
        ASSERT_EQ(v5->getNext()->getId(), 6);
        ASSERT_EQ(v6->getNext(), nullptr);

        ASSERT_EQ(v4->getPrev(), nullptr);
        ASSERT_EQ(v5->getPrev()->getId(), 4);
        ASSERT_EQ(v6->getPrev()->getId(), 5);

        ASSERT_EQ(v6->getBB()->getId(), 2);
    }

    // Fill bb3
    auto *v7 = new PhiInst{7};
    v7->addInput(std::make_pair(v5, bb2));
    auto *v8 = new BinaryInst{8, BinOpType::Cmp, v7, v6};
    auto *v9 = new JumpInst{9, JumpOpType::Ja, bb5};

    bb3->pushBackInst(v7);
    bb3->pushBackInst(v8);
    bb3->pushBackInst(v9);

    {
        ASSERT_EQ(v7->getId(), 7);
        ASSERT_EQ(v8->getId(), 8);
        ASSERT_EQ(v9->getId(), 9);

        ASSERT_EQ(v7->getNext()->getId(), 8);
        ASSERT_EQ(v8->getNext()->getId(), 9);
        ASSERT_EQ(v9->getNext(), nullptr);

        ASSERT_EQ(v7->getPrev(), nullptr);
        ASSERT_EQ(v8->getPrev()->getId(), 7);
        ASSERT_EQ(v9->getPrev()->getId(), 8);

        ASSERT_EQ(v9->getBB()->getId(), 3);
    }

    // Fill bb4
    auto *v10 = new PhiInst{10};
    v10->addInput(std::make_pair(v4, bb2));
    auto *v11 = new PhiInst{11};
    v11->addInput(std::make_pair(v5, bb2));
    auto *v12 = new BinaryInst{12, BinOpType::Mul, v10, v11};
    auto *v13 = new BinaryInst{13, BinOpType::Add, v11, v2};
    auto *v14 = new JumpInst{14, JumpOpType::Jmp, bb3};

    v7->addInput(std::make_pair(v13, bb4));
    v10->addInput(std::make_pair(v12, bb4));
    v11->addInput(std::make_pair(v13, bb4));

    bb4->pushBackInst(v10);
    bb4->pushBackInst(v11);
    bb4->pushBackInst(v12);
    bb4->pushBackInst(v13);
    bb4->pushBackInst(v14);

    {
        ASSERT_EQ(v10->getId(), 10);
        ASSERT_EQ(v11->getId(), 11);
        ASSERT_EQ(v12->getId(), 12);
        ASSERT_EQ(v13->getId(), 13);
        ASSERT_EQ(v14->getId(), 14);

        ASSERT_EQ(v10->getNext()->getId(), 11);
        ASSERT_EQ(v11->getNext()->getId(), 12);
        ASSERT_EQ(v12->getNext()->getId(), 13);
        ASSERT_EQ(v13->getNext()->getId(), 14);
        ASSERT_EQ(v14->getNext(), nullptr);

        ASSERT_EQ(v10->getPrev(), nullptr);
        ASSERT_EQ(v11->getPrev()->getId(), 10);
        ASSERT_EQ(v12->getPrev()->getId(), 11);
        ASSERT_EQ(v13->getPrev()->getId(), 12);
        ASSERT_EQ(v14->getPrev()->getId(), 13);

        ASSERT_EQ(v14->getBB()->getId(), 4);
    }

    // Fill bb5
    auto *v15 = new PhiInst{15};
    v15->addInput(std::make_pair(v4, bb2));
    v15->addInput(std::make_pair(v12, bb4));
    auto *v16 = new UnaryInst{16, UnOpType::Return, v15};

    bb5->pushBackInst(v15);
    bb5->pushBackInst(v16);

    {
        ASSERT_EQ(v15->getId(), 15);
        ASSERT_EQ(v16->getId(), 16);

        ASSERT_EQ(v15->getNext()->getId(), 16);
        ASSERT_EQ(v16->getNext(), nullptr);

        ASSERT_EQ(v15->getPrev(), nullptr);
        ASSERT_EQ(v16->getPrev()->getId(), 15);

        ASSERT_EQ(v16->getBB()->getId(), 5);
    }

    graph->insertBB(bb1);
    graph->insertBB(bb2);
    graph->insertBB(bb3);
    graph->insertBB(bb4);
    graph->insertBB(bb5);

    // graph->dump();

    {
        ASSERT_EQ(bb1->getId(), 1);
        ASSERT_EQ(bb2->getId(), 2);
        ASSERT_EQ(bb3->getId(), 3);
        ASSERT_EQ(bb4->getId(), 4);
        ASSERT_EQ(bb5->getId(), 5);

        ASSERT_EQ(bb1->getTrueSucc()->getId(), 2);
        ASSERT_EQ(bb2->getTrueSucc()->getId(), 3);
        ASSERT_EQ(bb3->getTrueSucc()->getId(), 4);
        ASSERT_EQ(bb4->getTrueSucc()->getId(), 5);
        ASSERT_EQ(bb5->getTrueSucc(), nullptr);
        ASSERT_EQ(bb5->getFalseSucc(), nullptr);

        ASSERT_EQ(bb1->getPreds().size(), 0);
        ASSERT_EQ(bb2->getPreds()[0]->getId(), 1);
        ASSERT_EQ(bb3->getPreds()[0]->getId(), 2);
        ASSERT_EQ(bb4->getPreds()[0]->getId(), 3);
        ASSERT_EQ(bb5->getPreds()[0]->getId(), 4);

        ASSERT_EQ(bb5->getGraph(), graph);
    }
}
