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
 *             |          |     |
 *             |          v     |
 *             |         [5]----/
 *             |          |
 *             |          v
 *             \-------->[6]
 */
TEST(RPO_TEST, TEST1)
{
    auto graph = std::make_shared<Graph>("rpo_test1");

    auto* bb1 = new BasicBlock{1, graph};
    auto* bb2 = new BasicBlock{2, graph};
    auto* bb3 = new BasicBlock{3, graph};
    auto* bb4 = new BasicBlock{4, graph};
    auto* bb5 = new BasicBlock{5, graph};
    auto* bb6 = new BasicBlock{6, graph};

    graph->insertBB(bb1);
    graph->insertBB(bb2);
    graph->insertBBAfter(bb2, bb3, true);
    graph->insertBBAfter(bb2, bb4, false);
    graph->insertBBAfter(bb4, bb5);
    graph->insertBBAfter(bb5, bb6, false);
    bb5->addSucc(bb2);
    graph->addEdge(bb3, bb6);
    // graph->dump();

    graph->runPassRpo();

    auto& bbs = graph->getRpoBBs();

    ASSERT_EQ(bbs.size(), 6);
    ASSERT_EQ(bbs[0]->getId(), 1);
    ASSERT_EQ(bbs[1]->getId(), 2);
    ASSERT_EQ(bbs[2]->getId(), 4);
    ASSERT_EQ(bbs[3]->getId(), 5);
    ASSERT_EQ(bbs[4]->getId(), 3);
    ASSERT_EQ(bbs[5]->getId(), 6);
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
    // graph->dump();

    graph->runPassRpo();

    auto& bbs = graph->getRpoBBs();

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
 *         |        [7]--------/   |
 *         |         |             |
 *         |         v             |
 *         |        [8]------------/
 *         |         |
 *         |         v
 *         \------->[9]
 *
 */
TEST(RPO_TEST, TEST3)
{
    auto graph = std::make_shared<Graph>("rpo_test3");

    auto* bb1 = new BasicBlock{1, graph};
    auto* bb2 = new BasicBlock{2, graph};
    auto* bb3 = new BasicBlock{3, graph};
    auto* bb4 = new BasicBlock{4, graph};
    auto* bb5 = new BasicBlock{5, graph};
    auto* bb6 = new BasicBlock{6, graph};
    auto* bb7 = new BasicBlock{7, graph};
    auto* bb8 = new BasicBlock{8, graph};
    auto* bb9 = new BasicBlock{9, graph};

    graph->insertBB(bb1);
    graph->insertBB(bb2);
    graph->insertBBAfter(bb2, bb3, true);
    graph->insertBBAfter(bb2, bb4, false);
    graph->insertBBAfter(bb3, bb5, true);
    graph->insertBBAfter(bb3, bb6, false);
    graph->addEdge(bb4, bb5);
    graph->insertBBAfter(bb5, bb7, true);
    graph->insertBBAfter(bb7, bb8, true);
    graph->insertBBAfter(bb8, bb9, false);
    graph->addEdge(bb7, bb2);
    graph->addEdge(bb8, bb1);
    graph->addEdge(bb6, bb9);
    // graph->dump();

    graph->runPassRpo();

    auto& bbs = graph->getRpoBBs();

    ASSERT_EQ(bbs.size(), 9);
    ASSERT_EQ(bbs[0]->getId(), 1);
    ASSERT_EQ(bbs[1]->getId(), 2);
    ASSERT_EQ(bbs[2]->getId(), 4);
    ASSERT_EQ(bbs[3]->getId(), 3);
    ASSERT_EQ(bbs[4]->getId(), 6);
    ASSERT_EQ(bbs[5]->getId(), 5);
    ASSERT_EQ(bbs[6]->getId(), 7);
    ASSERT_EQ(bbs[7]->getId(), 8);
    ASSERT_EQ(bbs[8]->getId(), 9);
}

/**
 * Test4 graph:
 *                   [1]
 *                    |
 *                    v
 *             /-----[2]-----\
 *             |             |
 *             v             v
 *            [3]    [5]<---[6]
 *             |      |      |
 *             |      v      v
 *             \---->[4]<---[7]
 */
TEST(RPO_TEST, TEST4)
{
    auto graph = std::make_shared<Graph>("rpo_test4");

    auto* bb1 = new BasicBlock{1, graph};
    auto* bb2 = new BasicBlock{2, graph};
    auto* bb3 = new BasicBlock{3, graph};
    auto* bb4 = new BasicBlock{4, graph};
    auto* bb5 = new BasicBlock{5, graph};
    auto* bb6 = new BasicBlock{6, graph};
    auto* bb7 = new BasicBlock{7, graph};

    graph->insertBB(bb1);
    graph->insertBB(bb2);
    graph->insertBB(bb3);
    graph->insertBB(bb4);
    graph->insertBBAfter(bb2, bb6, false);
    graph->insertBBAfter(bb6, bb5, true);
    graph->insertBBAfter(bb6, bb7, false);
    graph->addEdge(bb5, bb4);
    graph->addEdge(bb7, bb4);
    // graph->dump();

    graph->runPassRpo();

    auto& bbs = graph->getRpoBBs();

    ASSERT_EQ(bbs.size(), 7);
    ASSERT_EQ(bbs[0]->getId(), 1);
    ASSERT_EQ(bbs[1]->getId(), 2);
    ASSERT_EQ(bbs[2]->getId(), 6);
    ASSERT_EQ(bbs[3]->getId(), 7);
    ASSERT_EQ(bbs[4]->getId(), 5);
    ASSERT_EQ(bbs[5]->getId(), 3);
    ASSERT_EQ(bbs[6]->getId(), 4);
}

/**
 * Test5 graph:
 *                  [1]
 *                   |
 *                   v
 *          /------>[2]----\
 *          |        |     |
 *          |        v     v
 *          |    /->[3]<--[11]
 *          |    |   |
 *          |    |   v
 *          |    \--[4]
 *          |        |
 *          |        v
 *          |       [5]<--\
 *          |        |    |
 *          |        v    |
 *          |       [6]---/
 *          |        |
 *          |        v
 *         [8]<-----[7]--->[9]
 *                          |
 *                          v
 *                         [10]
 */
TEST(RPO_TEST, TEST5)
{
    auto graph = std::make_shared<Graph>("rpo_test5");

    auto* bb1 = new BasicBlock{1, graph};
    auto* bb2 = new BasicBlock{2, graph};
    auto* bb3 = new BasicBlock{3, graph};
    auto* bb4 = new BasicBlock{4, graph};
    auto* bb5 = new BasicBlock{5, graph};
    auto* bb6 = new BasicBlock{6, graph};
    auto* bb7 = new BasicBlock{7, graph};
    auto* bb8 = new BasicBlock{8, graph};
    auto* bb9 = new BasicBlock{9, graph};
    auto* bb10 = new BasicBlock{10, graph};
    auto* bb11 = new BasicBlock{11, graph};

    graph->insertBB(bb1);
    graph->insertBB(bb2);
    graph->insertBB(bb3);
    graph->insertBB(bb4);
    graph->insertBB(bb5);
    graph->insertBB(bb6);
    graph->insertBB(bb7);
    graph->insertBB(bb8);
    graph->insertBBAfter(bb7, bb9, false);
    graph->insertBBAfter(bb9, bb10, true);
    graph->insertBBAfter(bb2, bb11, false);
    graph->addEdge(bb4, bb3);
    graph->addEdge(bb6, bb5);
    graph->addEdge(bb8, bb2);
    graph->addEdge(bb11, bb3);
    // graph->dump();

    graph->runPassRpo();

    auto& bbs = graph->getRpoBBs();

    ASSERT_EQ(bbs.size(), 11);
    ASSERT_EQ(bbs[0]->getId(), 1);
    ASSERT_EQ(bbs[1]->getId(), 2);
    ASSERT_EQ(bbs[2]->getId(), 11);
    ASSERT_EQ(bbs[3]->getId(), 3);
    ASSERT_EQ(bbs[4]->getId(), 4);
    ASSERT_EQ(bbs[5]->getId(), 5);
    ASSERT_EQ(bbs[6]->getId(), 6);
    ASSERT_EQ(bbs[7]->getId(), 7);
    ASSERT_EQ(bbs[8]->getId(), 9);
    ASSERT_EQ(bbs[9]->getId(), 10);
    ASSERT_EQ(bbs[10]->getId(), 8);
}

/**
 * Test6 graph:
 *                  [1]
 *                   |
 *                   v
 *          /------>[2]----\
 *          |        |     |
 *          |        v     v
 *          |    /--[5]   [3]<--\
 *          |    |   |     |    |
 *          |    |   |     v    |
 *          |    |   \--->[4]   |
 *          |    v         |    |
 *          \---[6]        |    |
 *               |         |    |
 *               v         v    |
 *              [8]------>[7]---/
 *               |         |
 *               |         v
 *               \------->[9]
 */
TEST(RPO_TEST, TEST6)
{
    auto graph = std::make_shared<Graph>("rpo_test6");

    auto* bb1 = new BasicBlock{1, graph};
    auto* bb2 = new BasicBlock{2, graph};
    auto* bb3 = new BasicBlock{3, graph};
    auto* bb4 = new BasicBlock{4, graph};
    auto* bb5 = new BasicBlock{5, graph};
    auto* bb6 = new BasicBlock{6, graph};
    auto* bb7 = new BasicBlock{7, graph};
    auto* bb8 = new BasicBlock{8, graph};
    auto* bb9 = new BasicBlock{9, graph};

    graph->insertBB(bb1);
    graph->insertBB(bb2);
    graph->insertBB(bb3);
    graph->insertBB(bb4);
    graph->insertBBAfter(bb2, bb5, false);
    graph->insertBBAfter(bb5, bb6, true);
    graph->insertBBAfter(bb4, bb7, true);
    graph->insertBBAfter(bb6, bb8, true);
    graph->insertBBAfter(bb8, bb9, true);
    graph->addEdge(bb5, bb4);
    graph->addEdge(bb6, bb2);
    graph->addEdge(bb7, bb9);
    graph->addEdge(bb7, bb3);
    // graph->dump();

    graph->runPassRpo();

    auto& bbs = graph->getRpoBBs();

    ASSERT_EQ(bbs.size(), 9);
    ASSERT_EQ(bbs[0]->getId(), 1);
    ASSERT_EQ(bbs[1]->getId(), 2);
    ASSERT_EQ(bbs[2]->getId(), 5);
    ASSERT_EQ(bbs[3]->getId(), 6);
    ASSERT_EQ(bbs[4]->getId(), 8);
    ASSERT_EQ(bbs[5]->getId(), 3);
    ASSERT_EQ(bbs[6]->getId(), 4);
    ASSERT_EQ(bbs[7]->getId(), 7);
    ASSERT_EQ(bbs[8]->getId(), 9);
}