#include <gtest/gtest.h>

#include "../src/AlgorithmNode.hpp"

/*
 * Note: Unit tests should not depend on roscore to be active in order to run, or depend on a config file to be loaded.
 *       If you require either of these two things you should create a rostest instead.
 */

TEST(AlgorithmNode, hello)
{
    auto my_node = AlgorithmNode();
    (void) my_node; // rm unused variable err
    EXPECT_EQ(0, my_node.zero());
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
