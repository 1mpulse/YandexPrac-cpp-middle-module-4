#include "metric_accumulator_impl/categorical_accumulator.hpp"

#include <gtest/gtest.h>

#include <string>
#include <stdexcept>

namespace analyzer::metric_accumulator::metric_accumulator_impl::test {

TEST(CategoricalAccumulatorTest, Test1) {
    CategoricalAccumulator accumulator;

    accumulator.Accumulate(metric::MetricResult{"Naming style", std::string("Snake Case")});
    accumulator.Accumulate(metric::MetricResult{"Naming style", std::string("Snake Case")});
    accumulator.Accumulate(metric::MetricResult{"Naming style", std::string("Camel Case")});
    accumulator.Finalize();

    const auto &result = accumulator.Get();
    EXPECT_EQ(result.at("Snake Case"), 2);
    EXPECT_EQ(result.at("Camel Case"), 1);
}

TEST(CategoricalAccumulatorTest, Tes2) {
    CategoricalAccumulator accumulator;

    EXPECT_THROW(accumulator.Get(), std::runtime_error);
}

TEST(CategoricalAccumulatorTest, Test3) {
    CategoricalAccumulator accumulator;

    EXPECT_THROW(accumulator.Accumulate(metric::MetricResult{"Cyclomatic Complexity", 4}), std::runtime_error);
}

TEST(CategoricalAccumulatorTest, Test44) {
    CategoricalAccumulator accumulator;

    accumulator.Accumulate(metric::MetricResult{"Naming style", std::string("Snake Case")});
    accumulator.Finalize();
    accumulator.Reset();

    EXPECT_THROW(accumulator.Get(), std::runtime_error);
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test
