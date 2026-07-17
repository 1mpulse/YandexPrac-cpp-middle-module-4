#include "metric_impl/code_lines_count.hpp"

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

TEST(CodeLinesCountMetricTest, Test1) {
    CodeLinesCountMetric metric;
    function::Function function = LoadFirstFunctionFromFile("simple.py");

    EXPECT_EQ(std::get<int>(metric.Calculate(function).value), 5);
}

TEST(CodeLinesCountMetricTest, Test2) {
    CodeLinesCountMetric metric;
    function::Function function = LoadFirstFunctionFromFile("comments.py");

    EXPECT_EQ(std::get<int>(metric.Calculate(function).value), 3);
}

}  // namespace analyzer::metric::metric_impl
