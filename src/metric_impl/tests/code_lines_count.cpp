#include "metric_impl/code_lines_count.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

#include "file.hpp"
#include "function.hpp"

namespace analyzer::metric::metric_impl {

TEST(CodeLinesCountMetricTest, Test1) {
    CodeLinesCountMetric metric;
    function::Function function = function::LoadFirstFunctionFromFile("simple.py");

    EXPECT_EQ(std::get<int>(metric.Calculate(function).value), 5);
}

TEST(CodeLinesCountMetricTest, Test2) {
    CodeLinesCountMetric metric;
    function::Function function = function::LoadFirstFunctionFromFile("comments.py");

    EXPECT_EQ(std::get<int>(metric.Calculate(function).value), 3);
}

}  // namespace analyzer::metric::metric_impl
