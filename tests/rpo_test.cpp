#include "ir/graph.h"
#include "pass/rpo.h"
#include "gtest/gtest.h"

using namespace compiler;

/**
 * Test1 graph:
 *                 [1]
 *                  |
 *                  v
 *             /---[2]<---------\
 *             |    |           |
 *             v    |           |
 *            [3]   \--->[4]    |
 *                        |     |
 *                        v     |
 *                       [5]----/
 */
TEST(RPO_TEST, TEST1)
{
    auto graph = std::make_shared<Graph>("rpo_test1");

    auto *bb1 = new BasicBlock{1, graph};
    auto *bb2 = new BasicBlock{2, graph};
    auto *bb3 = new BasicBlock{3, graph};
    auto *bb4 = new BasicBlock{4, graph};
    auto *bb5 = new BasicBlock{5, graph};

    graph->insertBB(bb1);
    graph->insertBB(bb2);
    graph->insertBBAfter(bb2, bb3, true);
    graph->insertBBAfter(bb2, bb4, false);
    graph->insertBBAfter(bb4, bb5);
    bb5->addSucc(bb2);
    // graph->dump();

    graph->runPassRpo();

    auto bbs = graph->getRpoBBs();

    ASSERT_EQ(bbs.size(), 5);
    ASSERT_EQ(bbs[0]->getId(), 1);
    ASSERT_EQ(bbs[1]->getId(), 2);
    ASSERT_EQ(bbs[2]->getId(), 4);
    ASSERT_EQ(bbs[3]->getId(), 5);
    ASSERT_EQ(bbs[4]->getId(), 3);
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
TEST(RPO_TEST, TEST2)
{
    auto graph = std::make_shared<Graph>("rpo_test2");

    auto *bb1 = new BasicBlock{1, graph};
    auto *bb2 = new BasicBlock{2, graph};
    auto *bb3 = new BasicBlock{3, graph};
    auto *bb4 = new BasicBlock{4, graph};
    auto *bb5 = new BasicBlock{5, graph};
    auto *bb6 = new BasicBlock{6, graph};

    graph->insertBB(bb1);
    graph->insertBB(bb2);
    graph->insertBB(bb3);
    graph->insertBBAfter(bb3, bb4, true);
    graph->insertBBAfter(bb3, bb5, false);
    graph->insertBBAfter(bb4, bb6, true);
    graph->addEdge(bb4, bb5);
    graph->addEdge(bb6, bb2);
    // graph->dump();

    graph->runPassRpo();

    auto bbs = graph->getRpoBBs();

    ASSERT_EQ(bbs.size(), 6);
    ASSERT_EQ(bbs[0]->getId(), 1);
    ASSERT_EQ(bbs[1]->getId(), 2);
    ASSERT_EQ(bbs[2]->getId(), 3);
    ASSERT_EQ(bbs[3]->getId(), 4);
    ASSERT_EQ(bbs[4]->getId(), 5);
    ASSERT_EQ(bbs[5]->getId(), 6);
}

/**
 * Test3 graph:
 *                  [1]<-----------\
 *                   |             |
 *                   v             |
 *                  [2]<-------\   |
 *                   |         |   |
 *                   |         |   |
 *         /--[3]<--/\--->[4]  |   |
 *         |   |           |   |   |
 *         |   \--->[5]<---/   |   |
 *         v         |         |   |
 *        [6]        v         |   |
 *                  [7]--------/   |
 *                   |             |
 *                   v             |
 *                  [8]------------/
 *
 */
TEST(RPO_TEST, TEST3)
{
    auto graph = std::make_shared<Graph>("rpo_test3");

    auto *bb1 = new BasicBlock{1, graph};
    auto *bb2 = new BasicBlock{2, graph};
    auto *bb3 = new BasicBlock{3, graph};
    auto *bb4 = new BasicBlock{4, graph};
    auto *bb5 = new BasicBlock{5, graph};
    auto *bb6 = new BasicBlock{6, graph};
    auto *bb7 = new BasicBlock{7, graph};
    auto *bb8 = new BasicBlock{8, graph};

    graph->insertBB(bb1);
    graph->insertBB(bb2);
    graph->insertBBAfter(bb2, bb3, true);
    graph->insertBBAfter(bb2, bb4, false);
    graph->insertBBAfter(bb3, bb5, true);
    graph->insertBBAfter(bb3, bb6, false);
    graph->addEdge(bb4, bb5);
    graph->insertBBAfter(bb5, bb7, true);
    graph->insertBBAfter(bb7, bb8, true);
    graph->addEdge(bb7, bb2);
    graph->addEdge(bb8, bb1);
    // graph->dump();

    graph->runPassRpo();

    auto bbs = graph->getRpoBBs();

    ASSERT_EQ(bbs.size(), 8);
    ASSERT_EQ(bbs[0]->getId(), 1);
    ASSERT_EQ(bbs[1]->getId(), 2);
    ASSERT_EQ(bbs[2]->getId(), 4);
    ASSERT_EQ(bbs[3]->getId(), 3);
    ASSERT_EQ(bbs[4]->getId(), 6);
    ASSERT_EQ(bbs[5]->getId(), 5);
    ASSERT_EQ(bbs[6]->getId(), 7);
    ASSERT_EQ(bbs[7]->getId(), 8);
}