#include "metric_accumulator_impl/sum_average_accumulator.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <string>

namespace analyzer::metric_accumulator::metric_accumulator_impl::test {

TEST(SumAverageAccumulatorTest, Test1) {
    SumAverageAccumulator accumulator;

    accumulator.Accumulate(metric::MetricResult{"Code lines count", 3});
    accumulator.Accumulate(metric::MetricResult{"Code lines count", 5});
    accumulator.Finalize();

    SumAverageAccumulator::SumAverage result = accumulator.Get();
    EXPECT_EQ(result.sum, 8);
    EXPECT_NEAR(result.average, 4.0, 1e-9);
}

TEST(SumAverageAccumulatorTest, Test2) {
    SumAverageAccumulator accumulator;

    EXPECT_THROW(accumulator.Get(), std::runtime_error);
}

TEST(SumAverageAccumulatorTest, Test3) {
    SumAverageAccumulator accumulator;

    EXPECT_THROW(accumulator.Accumulate(metric::MetricResult{"Naming style", std::string("Snake Case")}),
                 std::runtime_error);
}

TEST(SumAverageAccumulatorTest, Test4) {
    SumAverageAccumulator accumulator;

    accumulator.Accumulate(metric::MetricResult{"Code lines count", 10});
    accumulator.Finalize();
    accumulator.Reset();

    EXPECT_THROW(accumulator.Get(), std::runtime_error);
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test
