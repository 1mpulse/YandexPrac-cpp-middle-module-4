#include "metric_impl/cyclomatic_complexity.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

#include "file.hpp"
#include "function.hpp"

namespace analyzer::metric::metric_impl {

function::Function LoadFirstFunctionFromFile(const std::string &filename)
{
    file::File file(std::string(METRIC_TEST_FILES_DIR) + "/" + filename);
    function::FunctionExtractor extractor;
    std::vector<function::Function> functions = extractor.Get(file);

    if (functions.empty())
    {
        throw std::runtime_error("No functions found in file '" + filename + "'");
    }

    return functions.front();
}

TEST(CyclomaticComplexityMetricTest, Test1) {
    CyclomaticComplexityMetric metric;
    function::Function function = LoadFirstFunctionFromFile("if.py");

    EXPECT_EQ(std::get<int>(metric.Calculate(function).value), 2);
}

TEST(CyclomaticComplexityMetricTest, Test2) {
    CyclomaticComplexityMetric metric;
    function::Function function = LoadFirstFunctionFromFile("loops.py");

    EXPECT_EQ(std::get<int>(metric.Calculate(function).value), 4);
}

}  // namespace analyzer::metric::metric_impl
