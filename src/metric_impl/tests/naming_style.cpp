#include "metric_impl/naming_style.hpp"

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

TEST(NamingStyleMetricTest, Test1) {
    NamingStyleMetric metric;
    function::Function function = LoadFirstFunctionFromFile("simple.py");

    EXPECT_EQ(std::get<std::string>(metric.Calculate(function).value), "Snake Case");
}

TEST(NamingStyleMetricTest, Test2) {
    NamingStyleMetric metric;
    function::Function function = LoadFirstFunctionFromFile("if.py");

    EXPECT_EQ(std::get<std::string>(metric.Calculate(function).value), "Camel Case");
}

TEST(NamingStyleMetricTest, Test3) {
    NamingStyleMetric metric;
    function::Function function = LoadFirstFunctionFromFile("ternary.py");

    EXPECT_EQ(std::get<std::string>(metric.Calculate(function).value), "Unknown");
}

}  // namespace analyzer::metric::metric_impl
