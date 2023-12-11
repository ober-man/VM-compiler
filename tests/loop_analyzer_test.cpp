#include "ir/graph.h"
#include "pass/loop_analysis.h"
#include "gtest/gtest.h"

using namespace compiler;

void checkLoopBlocks(Loop* loop, std::vector<int> expected)
{
    auto& bbs = loop->getBody();
    size_t size = bbs.size();
    ASSERT_EQ(size, expected.size());

    auto compare = [](BasicBlock* bb1, BasicBlock* bb2) { return bb1->getId() < bb2->getId(); };
    std::sort(bbs.begin(), bbs.end(), compare);
    std::sort(expected.begin(), expected.end());

    for (size_t i = 0; i < size; ++i)
        ASSERT_EQ(bbs[i]->getId(), expected[i]);
}

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
TEST(LOOP_TEST, TEST1)
{
    auto graph = std::make_shared<Graph>("loop_test1");

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

    graph->runPassLoopAnalysis();

    auto* root = graph->getRootLoop();
    ASSERT_EQ(root->getOuterLoop(), nullptr);
    ASSERT_EQ(root->getHeader(), nullptr);
    ASSERT_EQ(root->getLatches().size(), 0);
    checkLoopBlocks(root, {1, 3, 6});

    auto& inner = root->getInnerLoops();
    ASSERT_EQ(inner.size(), 1);

    auto loop_5_2 = inner[0];
    ASSERT_EQ(loop_5_2->getOuterLoop(), root);
    ASSERT_EQ(loop_5_2->getInnerLoops().size(), 0);
    ASSERT_EQ(loop_5_2->getHeader()->getId(), 2);
    ASSERT_EQ(loop_5_2->getLatches()[0]->getId(), 5);
    checkLoopBlocks(loop_5_2, {2, 4, 5});
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
TEST(LOOP_TEST, TEST2)
{
    auto graph = std::make_shared<Graph>("loop_test2");

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

    graph->runPassLoopAnalysis();

    auto* root = graph->getRootLoop();
    ASSERT_EQ(root->getOuterLoop(), nullptr);
    ASSERT_EQ(root->getHeader(), nullptr);
    ASSERT_EQ(root->getLatches().size(), 0);
    checkLoopBlocks(root, {1, 5});

    auto& inner = root->getInnerLoops();
    ASSERT_EQ(inner.size(), 1);

    auto loop_6_2 = inner[0];
    ASSERT_EQ(loop_6_2->getOuterLoop(), root);
    ASSERT_EQ(loop_6_2->getInnerLoops().size(), 0);
    ASSERT_EQ(loop_6_2->getHeader()->getId(), 2);
    ASSERT_EQ(loop_6_2->getLatches()[0]->getId(), 6);
    checkLoopBlocks(loop_6_2, {2, 3, 4, 6});
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
 */
TEST(LOOP_TEST, TEST3)
{
    auto graph = std::make_shared<Graph>("loop_test3");

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

    graph->runPassLoopAnalysis();

    auto* root = graph->getRootLoop();
    ASSERT_EQ(root->getOuterLoop(), nullptr);
    ASSERT_EQ(root->getHeader(), nullptr);
    ASSERT_EQ(root->getLatches().size(), 0);
    checkLoopBlocks(root, {6, 9});

    auto& inner = root->getInnerLoops();
    ASSERT_EQ(inner.size(), 1);

    auto loop_8_1 = inner[0];
    ASSERT_EQ(loop_8_1->getOuterLoop(), root);
    ASSERT_EQ(loop_8_1->getInnerLoops().size(), 1);
    ASSERT_EQ(loop_8_1->getHeader()->getId(), 1);
    ASSERT_EQ(loop_8_1->getLatches()[0]->getId(), 8);
    checkLoopBlocks(loop_8_1, {1, 2, 8});

    auto loop_7_2 = loop_8_1->getInnerLoops()[0];
    ASSERT_EQ(loop_7_2->getOuterLoop(), loop_8_1);
    ASSERT_EQ(loop_7_2->getInnerLoops().size(), 0);
    ASSERT_EQ(loop_7_2->getHeader()->getId(), 2);
    ASSERT_EQ(loop_7_2->getLatches()[0]->getId(), 7);
    checkLoopBlocks(loop_7_2, {2, 3, 4, 5, 7});
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
TEST(LOOP_TEST, TEST4)
{
    auto graph = std::make_shared<Graph>("loop_test4");

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

    graph->runPassLoopAnalysis();

    auto* root = graph->getRootLoop();
    ASSERT_EQ(root->getOuterLoop(), nullptr);
    ASSERT_EQ(root->getHeader(), nullptr);
    ASSERT_EQ(root->getLatches().size(), 0);
    checkLoopBlocks(root, {1, 2, 3, 4, 5, 6, 7});

    auto& inner = root->getInnerLoops();
    ASSERT_EQ(inner.size(), 0);
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
TEST(LOOP_TEST, TEST5)
{
    auto graph = std::make_shared<Graph>("loop_test5");

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

    graph->runPassLoopAnalysis();

    auto* root = graph->getRootLoop();
    ASSERT_EQ(root->getOuterLoop(), nullptr);
    ASSERT_EQ(root->getHeader(), nullptr);
    ASSERT_EQ(root->getLatches().size(), 0);
    checkLoopBlocks(root, {1, 9, 10});

    auto& inner = root->getInnerLoops();
    ASSERT_EQ(inner.size(), 1);

    auto loop_8_2 = inner[0];
    ASSERT_EQ(loop_8_2->getOuterLoop(), root);
    ASSERT_EQ(loop_8_2->getInnerLoops().size(), 2);
    ASSERT_EQ(loop_8_2->getHeader()->getId(), 2);
    ASSERT_EQ(loop_8_2->getLatches()[0]->getId(), 8);
    checkLoopBlocks(loop_8_2, {2, 3, 5, 7, 8, 11});

    auto loop_6_5 = loop_8_2->getInnerLoops()[0];
    ASSERT_EQ(loop_6_5->getOuterLoop(), loop_8_2);
    ASSERT_EQ(loop_6_5->getInnerLoops().size(), 0);
    ASSERT_EQ(loop_6_5->getHeader()->getId(), 5);
    ASSERT_EQ(loop_6_5->getLatches()[0]->getId(), 6);
    checkLoopBlocks(loop_6_5, {5, 6});

    auto loop_4_3 = loop_8_2->getInnerLoops()[1];
    ASSERT_EQ(loop_4_3->getOuterLoop(), loop_8_2);
    ASSERT_EQ(loop_4_3->getInnerLoops().size(), 0);
    ASSERT_EQ(loop_4_3->getHeader()->getId(), 3);
    ASSERT_EQ(loop_4_3->getLatches()[0]->getId(), 4);
    checkLoopBlocks(loop_4_3, {3, 4});
}

/**
 * Test6 graph:
 *                  [1]
 *                   |
 *                   v
 *          /------>[2]----\
 *          |        |     |
 *          |        v     v
 *          |    /--[5]   [3]<---\
 *          |    |   |     |     |
 *          |    |   |     v     |
 *          |    |   \--->[4]    |
 *          |    v         |     |
 *          \---[6]        |     |
 *               |         |     |
 *               v         v     |
 *              [8]------>[7]----/
 *               |         |
 *               |         v
 *               \------->[9]
 */
TEST(LOOP_TEST, TEST6)
{
    auto graph = std::make_shared<Graph>("loop_test6");

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

    graph->runPassLoopAnalysis();

    auto* root = graph->getRootLoop();
    ASSERT_EQ(root->getOuterLoop(), nullptr);
    ASSERT_EQ(root->getHeader(), nullptr);
    ASSERT_EQ(root->getLatches().size(), 0);
    checkLoopBlocks(root, {1, 4, 8, 9});

    auto& inner = root->getInnerLoops();
    ASSERT_EQ(inner.size(), 2);

    auto loop_6_2 = inner[0];
    ASSERT_EQ(loop_6_2->getOuterLoop(), root);
    ASSERT_EQ(loop_6_2->getInnerLoops().size(), 0);
    ASSERT_EQ(loop_6_2->getHeader()->getId(), 2);
    ASSERT_EQ(loop_6_2->getLatches()[0]->getId(), 6);
    checkLoopBlocks(loop_6_2, {2, 5, 6});

    auto loop_7_3 = inner[1];
    ASSERT_EQ(loop_7_3->getOuterLoop(), root);
    ASSERT_EQ(loop_7_3->getInnerLoops().size(), 0);
    ASSERT_TRUE(loop_7_3->isIrreducible());
    ASSERT_EQ(loop_7_3->getHeader()->getId(), 3);
    ASSERT_EQ(loop_7_3->getLatches()[0]->getId(), 7);
    checkLoopBlocks(loop_7_3, {3, 7});
}