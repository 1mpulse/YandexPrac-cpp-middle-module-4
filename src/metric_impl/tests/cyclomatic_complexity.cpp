#include "metric_impl/cyclomatic_complexity.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

#include "file.hpp"
#include "function.hpp"

namespace analyzer::metric::metric_impl {

TEST(CyclomaticComplexityMetricTest, Test1) {
    CyclomaticComplexityMetric metric;
    function::Function function = function::LoadFirstFunctionFromFile("if.py");

    EXPECT_EQ(std::get<int>(metric.Calculate(function).value), 2);
}

TEST(CyclomaticComplexityMetricTest, Test2) {
    CyclomaticComplexityMetric metric;
    function::Function function = function::LoadFirstFunctionFromFile("loops.py");

    EXPECT_EQ(std::get<int>(metric.Calculate(function).value), 4);
}

}  // namespace analyzer::metric::metric_impl
