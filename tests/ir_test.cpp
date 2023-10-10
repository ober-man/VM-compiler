#include "graph.h"
#include "pass/domtree.h"
#include "pass/rpo.h"
#include "gtest/gtest.h"

TEST(IR_TEST, TEST1)
{
    ASSERT_EQ(2 * 2, 4);
}
