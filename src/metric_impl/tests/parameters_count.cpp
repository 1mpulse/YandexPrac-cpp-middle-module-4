#include "metric_impl/parameters_count.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

#include "file.hpp"
#include "function.hpp"

namespace analyzer::metric::metric_impl {

TEST(CountParametersMetricTest, Test1) {
    CountParametersMetric metric;
    function::Function function = function::LoadFirstFunctionFromFile("simple.py");

    EXPECT_EQ(std::get<int>(metric.Calculate(function).value), 0);
}

TEST(CountParametersMetricTest, Test2) {
    CountParametersMetric metric;
    function::Function function = function::LoadFirstFunctionFromFile("many_parameters.py");

    EXPECT_EQ(std::get<int>(metric.Calculate(function).value), 5);
}

}  // namespace analyzer::metric::metric_impl
