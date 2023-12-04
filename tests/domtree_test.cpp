#include "ir/graph.h"
#include "pass/domtree.h"
#include "gtest/gtest.h"

using namespace compiler;

void checkDominators(BasicBlock* bb, std::vector<int> expected)
{
    auto& doms = bb->getDominators();
    size_t size = doms.size();
    ASSERT_EQ(size, expected.size());

    auto compare = [](BasicBlock* bb1, BasicBlock* bb2) { return bb1->getId() < bb2->getId(); };
    std::sort(doms.begin(), doms.end(), compare);
    std::sort(expected.begin(), expected.end());

    for (size_t i = 0; i < size; ++i)
        ASSERT_EQ(doms[i]->getId(), expected[i]);
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
TEST(DOMTREE_TEST, TEST1)
{
    auto graph = std::make_shared<Graph>("dom_tree_test1");

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

    graph->runPassDomTree();

    auto& bbs = graph->getRpoBBs();
    ASSERT_EQ(bbs.size(), 6);

    checkDominators(bb1, {1});
    checkDominators(bb2, {1, 2});
    checkDominators(bb3, {1, 2, 3});
    checkDominators(bb4, {1, 2, 4});
    checkDominators(bb5, {1, 2, 4, 5});
    checkDominators(bb6, {1, 2, 6});

    ASSERT_EQ(bb1->getIdom()->getId(), 1);
    ASSERT_EQ(bb2->getIdom()->getId(), 1);
    ASSERT_EQ(bb3->getIdom()->getId(), 2);
    ASSERT_EQ(bb4->getIdom()->getId(), 2);
    ASSERT_EQ(bb5->getIdom()->getId(), 4);
    ASSERT_EQ(bb6->getIdom()->getId(), 2);
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
TEST(DOMTREE_TEST, TEST2)
{
    auto graph = std::make_shared<Graph>("dom_tree_test2");

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

    graph->runPassDomTree();

    auto& bbs = graph->getRpoBBs();
    ASSERT_EQ(bbs.size(), 6);

    checkDominators(bb1, {1});
    checkDominators(bb2, {1, 2});
    checkDominators(bb3, {1, 2, 3});
    checkDominators(bb4, {1, 2, 3, 4});
    checkDominators(bb5, {1, 2, 3, 5});
    checkDominators(bb6, {1, 2, 3, 4, 6});

    ASSERT_EQ(bb1->getIdom()->getId(), 1);
    ASSERT_EQ(bb2->getIdom()->getId(), 1);
    ASSERT_EQ(bb3->getIdom()->getId(), 2);
    ASSERT_EQ(bb4->getIdom()->getId(), 3);
    ASSERT_EQ(bb5->getIdom()->getId(), 3);
    ASSERT_EQ(bb6->getIdom()->getId(), 4);
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
TEST(DOMTREE_TEST, TEST3)
{
    auto graph = std::make_shared<Graph>("dom_tree_test3");

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

    graph->runPassDomTree();

    auto& bbs = graph->getRpoBBs();
    ASSERT_EQ(bbs.size(), 9);

    checkDominators(bb1, {1});
    checkDominators(bb2, {1, 2});
    checkDominators(bb3, {1, 2, 3});
    checkDominators(bb4, {1, 2, 4});
    checkDominators(bb5, {1, 2, 5});
    checkDominators(bb6, {1, 2, 3, 6});
    checkDominators(bb7, {1, 2, 5, 7});
    checkDominators(bb8, {1, 2, 5, 7, 8});
    checkDominators(bb9, {1, 2, 9});

    ASSERT_EQ(bb1->getIdom()->getId(), 1);
    ASSERT_EQ(bb2->getIdom()->getId(), 1);
    ASSERT_EQ(bb3->getIdom()->getId(), 2);
    ASSERT_EQ(bb4->getIdom()->getId(), 2);
    ASSERT_EQ(bb5->getIdom()->getId(), 2);
    ASSERT_EQ(bb6->getIdom()->getId(), 3);
    ASSERT_EQ(bb7->getIdom()->getId(), 5);
    ASSERT_EQ(bb8->getIdom()->getId(), 7);
    ASSERT_EQ(bb9->getIdom()->getId(), 2);
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
TEST(DOMTREE_TEST, TEST4)
{
    auto graph = std::make_shared<Graph>("dom_tree_test4");

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

    graph->runPassDomTree();

    auto& bbs = graph->getRpoBBs();
    ASSERT_EQ(bbs.size(), 7);

    checkDominators(bb1, {1});
    checkDominators(bb2, {1, 2});
    checkDominators(bb3, {1, 2, 3});
    checkDominators(bb4, {1, 2, 4});
    checkDominators(bb5, {1, 2, 5, 6});
    checkDominators(bb6, {1, 2, 6});
    checkDominators(bb7, {1, 2, 6, 7});

    ASSERT_EQ(bb1->getIdom()->getId(), 1);
    ASSERT_EQ(bb2->getIdom()->getId(), 1);
    ASSERT_EQ(bb3->getIdom()->getId(), 2);
    ASSERT_EQ(bb4->getIdom()->getId(), 2);
    ASSERT_EQ(bb5->getIdom()->getId(), 6);
    ASSERT_EQ(bb6->getIdom()->getId(), 2);
    ASSERT_EQ(bb7->getIdom()->getId(), 6);
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
TEST(DOMTREE_TEST, TEST5)
{
    auto graph = std::make_shared<Graph>("dom_tree_test5");

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

    graph->runPassDomTree();

    auto& bbs = graph->getRpoBBs();
    ASSERT_EQ(bbs.size(), 11);

    checkDominators(bb1, {1});
    checkDominators(bb2, {1, 2});
    checkDominators(bb3, {1, 2, 3});
    checkDominators(bb4, {1, 2, 3, 4});
    checkDominators(bb5, {1, 2, 3, 4, 5});
    checkDominators(bb6, {1, 2, 3, 4, 5, 6});
    checkDominators(bb7, {1, 2, 3, 4, 5, 6, 7});
    checkDominators(bb8, {1, 2, 3, 4, 5, 6, 7, 8});
    checkDominators(bb9, {1, 2, 3, 4, 5, 6, 7, 9});
    checkDominators(bb10, {1, 2, 3, 4, 5, 6, 7, 9, 10});
    checkDominators(bb11, {1, 2, 11});

    ASSERT_EQ(bb1->getIdom()->getId(), 1);
    ASSERT_EQ(bb2->getIdom()->getId(), 1);
    ASSERT_EQ(bb3->getIdom()->getId(), 2);
    ASSERT_EQ(bb4->getIdom()->getId(), 3);
    ASSERT_EQ(bb5->getIdom()->getId(), 4);
    ASSERT_EQ(bb6->getIdom()->getId(), 5);
    ASSERT_EQ(bb7->getIdom()->getId(), 6);
    ASSERT_EQ(bb8->getIdom()->getId(), 7);
    ASSERT_EQ(bb9->getIdom()->getId(), 7);
    ASSERT_EQ(bb10->getIdom()->getId(), 9);
    ASSERT_EQ(bb11->getIdom()->getId(), 2);
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
TEST(DOMTREE_TEST, TEST6)
{
    auto graph = std::make_shared<Graph>("dom_tree_test6");

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
    graph->addEdge(bb8, bb7);
    // graph->dump();

    graph->runPassDomTree();

    auto& bbs = graph->getRpoBBs();
    ASSERT_EQ(bbs.size(), 9);

    checkDominators(bb1, {1});
    checkDominators(bb2, {1, 2});
    checkDominators(bb3, {1, 2, 3});
    checkDominators(bb4, {1, 2, 4});
    checkDominators(bb5, {1, 2, 5});
    checkDominators(bb6, {1, 2, 5, 6});
    checkDominators(bb7, {1, 2, 7});
    checkDominators(bb8, {1, 2, 5, 6, 8});
    checkDominators(bb9, {1, 2, 9});

    ASSERT_EQ(bb1->getIdom()->getId(), 1);
    ASSERT_EQ(bb2->getIdom()->getId(), 1);
    ASSERT_EQ(bb3->getIdom()->getId(), 2);
    ASSERT_EQ(bb4->getIdom()->getId(), 2);
    ASSERT_EQ(bb5->getIdom()->getId(), 2);
    ASSERT_EQ(bb6->getIdom()->getId(), 5);
    ASSERT_EQ(bb7->getIdom()->getId(), 2);
    ASSERT_EQ(bb8->getIdom()->getId(), 6);
    ASSERT_EQ(bb9->getIdom()->getId(), 2);
}