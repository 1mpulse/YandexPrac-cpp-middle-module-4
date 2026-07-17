#include "metric_accumulator_impl/average_accumulator.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <string>

namespace analyzer::metric_accumulator::metric_accumulator_impl::test {

TEST(AverageAccumulatorTest, Test1) {
    AverageAccumulator accumulator;

    accumulator.Accumulate(metric::MetricResult{"Parameters count", 2});
    accumulator.Accumulate(metric::MetricResult{"Parameters count", 4});
    accumulator.Accumulate(metric::MetricResult{"Parameters count", 6});
    accumulator.Finalize();

    EXPECT_NEAR(accumulator.Get(), 4.0, 1e-9);
}

TEST(AverageAccumulatorTest, Test2) {
    AverageAccumulator accumulator;

    EXPECT_THROW(accumulator.Get(), std::runtime_error);
}

TEST(AverageAccumulatorTest, Test3) {
    AverageAccumulator accumulator;

    EXPECT_THROW(accumulator.Accumulate(metric::MetricResult{"Naming style", std::string("Snake Case")}),
                 std::runtime_error);
}

TEST(AverageAccumulatorTest, Test4) {
    AverageAccumulator accumulator;

    accumulator.Accumulate(metric::MetricResult{"Parameters count", 3});
    accumulator.Finalize();
    accumulator.Reset();

    EXPECT_THROW(accumulator.Get(), std::runtime_error);
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test
