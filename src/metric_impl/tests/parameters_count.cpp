#include "metric_impl/parameters_count.hpp"

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

TEST(CountParametersMetricTest, Test1) {
    CountParametersMetric metric;
    function::Function function = LoadFirstFunctionFromFile("simple.py");

    EXPECT_EQ(std::get<int>(metric.Calculate(function).value), 0);
}

TEST(CountParametersMetricTest, Test2) {
    CountParametersMetric metric;
    function::Function function = LoadFirstFunctionFromFile("many_parameters.py");

    EXPECT_EQ(std::get<int>(metric.Calculate(function).value), 5);
}

}  // namespace analyzer::metric::metric_impl
