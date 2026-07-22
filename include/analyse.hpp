#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

#include "file.hpp"
#include "function.hpp"
#include "metric.hpp"
#include "metric_accumulator.hpp"

namespace analyzer {

namespace rv = std::ranges::views;
namespace rs = std::ranges;
/**
 * @brief Анализирует список Python-файлов и извлекает метрики для всех функций и методов.
 *
 * Эта функция — центральный "конвейер" обработки:
 * 1. Принимает имена файлов.
 * 2. Для каждого файла создаёт объект `File`, который автоматически парсит его через tree-sitter
 *    и строит AST.
 * 3. Извлекает из AST все функции и методы с помощью `FunctionExtractor`.
 * 4. Объединяет все функции из всех файлов в один плоский список (`join`).
 * 5. Для каждой функции вычисляет набор метрик через переданный `metric_extractor`.
 * 6. Возвращает вектор пар: (функция, результаты её метрик).
 */
auto AnalyseFunctions(const std::vector<std::string> &files,
                      const analyzer::metric::MetricExtractor &metric_extractor) {
    auto parsed_files = files |
                        rv::transform([](const auto &filename) { return file::File{filename}; }) |
                        rs::to<std::vector>();

    function::FunctionExtractor function_extractor;
    auto all_functions = parsed_files |
                         rv::transform([&](const auto &current_file) { return function_extractor.Get(current_file); }) |
                         rv::join |
                         rs::to<std::vector>();

    return all_functions |
           rv::transform([&](const function::Function &current_function) {
               return std::pair{current_function, metric_extractor.Get(current_function)};
           }) |
           rs::to<std::vector>();
}

/**
 * 
 * @brief Группирует результаты анализа по классам.
 *
 * Эта функция:
 * 1. Отфильтровывает только те функции, которые являются **методами классов**
 *    (у них `class_name.has_value()` == true).
 * 2. Группирует последовательные элементы с одинаковым именем класса с помощью `chunk_by`.
 *
 * Важно:
 * - `chunk_by` работает только с **последовательными** одинаковыми элементами!
 *   Поэтому предполагается, что входной диапазон уже упорядочен по классам
 *   (например, порядок методов в AST сохраняется как в исходном файле).
 * - Если порядок нарушен, один и тот же класс может быть разбит на несколько групп.
 *
 *  Чтобы убедиться, что фильтрация работает, проверьте, что свободные функции (без class_name)
 * действительно исчезают из результата.
 */
auto SplitByClasses(const auto &analysis) {
    using AnalysisElement = std::pair<function::Function, metric::MetricResults>;

    return analysis | rv::filter([](const AnalysisElement &current_element)
        {
            return current_element.first.class_name.has_value();
        }) |
        rv::chunk_by([](const AnalysisElement &lhs, const AnalysisElement &rhs)
        {
            return lhs.first.class_name == rhs.first.class_name;
        }) |
        rv::transform([](auto &&group) { return rs::to<std::vector<AnalysisElement>>(group); }) |
        rs::to<std::vector>();
}

/**
 * @brief Группирует результаты анализа по исходным файлам.
 *
 * Эта функция:
 * - Разбивает весь список функций на группы, где каждая группа содержит
 *   только функции из одного и того же файла (`filename`).
 * - Использует `chunk_by`, поэтому **порядок функций в `analysis` должен быть по файлам**.
 */
auto SplitByFiles(const auto &analysis) {
    using AnalysisElement = std::pair<function::Function, metric::MetricResults>;
    return analysis |
       rv::chunk_by([](const AnalysisElement &lhs, const AnalysisElement &rhs)
       {
           return lhs.first.filename == rhs.first.filename;
       }) |
       rv::transform([](auto &&group) { return rs::to<std::vector<AnalysisElement>>(group); }) |
       rs::to<std::vector>();
}

/**
 * @brief Агрегирует метрики всех функций с помощью аккумулятора.
 *
 * Эта функция:
 * - Проходит по каждому элементу результата `AnalyseFunctions`
 *   (то есть по каждой функции и её метрикам).
 * - Передаёт результаты метрик (`elem.second`) в аккумулятор через `AccumulateNextFunctionResults`.
 */
void AccumulateFunctionAnalysis(const auto &analysis,
                                const analyzer::metric_accumulator::MetricsAccumulator &accumulator) {
    std::ranges::for_each(analysis, [&](const auto &current_element)
    {
        accumulator.AccumulateNextFunctionResults(current_element.second);
    });
}

}  // namespace analyzer
