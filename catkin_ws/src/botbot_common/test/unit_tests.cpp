#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <string>



#include "botbot_common/CsvWriter.hpp"
#include "botbot_common/MathUtils.hpp"
#include "botbot_common/ThreadPool.hpp"
#include "botbot_common/VectorXY.hpp"


TEST(MathUtils, deg2rad) { EXPECT_DOUBLE_EQ(7.0 / 180.0 * mms::MathUtils::PI, mms::MathUtils::deg2rad(7.0)); }

TEST(MathUtils, rad2deg) { EXPECT_DOUBLE_EQ(0.7 / mms::MathUtils::PI * 180.0, mms::MathUtils::rad2deg(0.7)); }

TEST(MathUtils, norm)
{
    EXPECT_EQ(10, mms::MathUtils::norm(-10));
    EXPECT_DOUBLE_EQ(10., mms::MathUtils::norm(-10.));

    EXPECT_EQ(5, mms::MathUtils::norm(3, 4));
    EXPECT_DOUBLE_EQ(5., mms::MathUtils::norm(3., 4.));

    EXPECT_EQ(5, mms::MathUtils::norm(-3, 4));
    EXPECT_DOUBLE_EQ(5., mms::MathUtils::norm(3., -4.));

    EXPECT_EQ(29, mms::MathUtils::norm(12, -16, 21));
    EXPECT_DOUBLE_EQ(29., mms::MathUtils::norm(12., 16., -21.));

    EXPECT_EQ(2109, mms::MathUtils::norm(1040, -1041, -1042, 1094));
    EXPECT_DOUBLE_EQ(2109., mms::MathUtils::norm(-1040., -1041., -1042., -1094.));
}

TEST(VectorXY, operands)
{
    mms::VectorXY cone1(1.5, 1.5);
    mms::VectorXY cone2(-0.5, -2.5);

    cone1 += cone2;
    EXPECT_DOUBLE_EQ(cone1.x(), 1.0);
    EXPECT_DOUBLE_EQ(cone1.y(), -1.0);

    cone1 = cone2 - cone1;
    EXPECT_DOUBLE_EQ(cone1.x(), -1.5);
    EXPECT_DOUBLE_EQ(cone1.y(), -1.5);
}

TEST(VectorXY, basic_methods)
{
    mms::VectorXY cone1(3.0, -4.0);
    EXPECT_DOUBLE_EQ(cone1.norm(), 5.0);
    mms::VectorXY cone2(-13, 7);
    EXPECT_DOUBLE_EQ(cone1.dot(cone2), -67);
    mms::VectorXY cone3(-2, 2);
    EXPECT_NEAR(cone3.rotate(mms::MathUtils::PI / 2.0).x(), -2, 1e-6);
    EXPECT_NEAR(cone3.rotate(mms::MathUtils::PI / 2.0).y(), -2, 1e-6);
}

TEST(VectorXY, resolutes)
{
    // Not 100% sure these are all correct
    mms::VectorXY cone1(3.0, 4.0);
    EXPECT_DOUBLE_EQ(cone1.parallel_resolute({2, 0}), 3.0);
    EXPECT_DOUBLE_EQ(cone1.parallel_resolute({0, 6}), 4.0);
    EXPECT_DOUBLE_EQ(cone1.parallel_resolute({0, -20}), -4.0);
    EXPECT_EQ(cone1.parallel_projection({2, 0}), mms::VectorXY(3, 0));
    EXPECT_EQ(cone1.parallel_projection({0, -20}), mms::VectorXY(0, 4));

    EXPECT_EQ(cone1.perpendicular_projection({0, 100}), mms::VectorXY(3.0, 0));
    EXPECT_EQ(cone1.perpendicular_projection({-5, 0}), mms::VectorXY(0, 4));
    EXPECT_DOUBLE_EQ(cone1.perpendicular_resolute({2, 0}), 4.0);
    EXPECT_DOUBLE_EQ(cone1.perpendicular_resolute({0, 6}), 3.0);
    EXPECT_DOUBLE_EQ(cone1.perpendicular_resolute({0, -20}), 3.0);
}

TEST(ThreadPool, completion)
{
    int tr_sz = 4;
    mms::ThreadPool pool(tr_sz);
    auto dummy_func = [](int *a, int b) { *a = b; };
    std::vector<int> outs(tr_sz, 0);
    std::vector<int> expected(tr_sz, 1);

    for (int &out : outs)
    {
        pool.add_job(dummy_func, &out, 1);
    }

    pool.wait_for_completion();

    EXPECT_EQ(outs, expected);
}

TEST(CsvWriter, BasicWriter)
{
    mms::CsvWriter<int, int, std::optional<double>> writer{{"a", "b", "c"}, "unit_test.csv"};
    writer.log_row(1, 5, {});
}

TEST(CsvWriter, ExpectThrows)
{
    using Writer_t = mms::CsvWriter<int, int>;
    EXPECT_THROW((Writer_t{{"a"}, "unit_test"}), std::logic_error);
}

TEST(CsvWriter, VectorWriter)
{
    mms::CsvVectorWriter<int, 5> writer{{"a", "b", "c", "d", "e"}, "unit_test.csv"};
    writer.log_row(1, 2, 3, 4, 5);
    writer.log_vector_row(std::vector{1, 2, 3, 4, 5});
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
