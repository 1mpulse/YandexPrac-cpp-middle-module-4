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
    std::vector<file::File> parsed_files;
    parsed_files.reserve(files.size());

    for (const auto &filename : files)
    {
        parsed_files.emplace_back(filename);
    }

    function::FunctionExtractor function_extractor;
    std::vector<function::Function> all_functions;

    for (const file::File &current_file : parsed_files)
    {
        const auto file_functions = function_extractor.Get(current_file);
        for (const auto &current_function : file_functions)
        {
            all_functions.push_back(current_function);
        }
    }

    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
    analysis.reserve(all_functions.size());

    for (const function::Function &current_function : all_functions)
    {
        metric::MetricResults metric_results = metric_extractor.Get(current_function);
        analysis.emplace_back(current_function, metric_results);
    }

    return analysis;
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

    std::vector<std::vector<AnalysisElement>> result;

    for (const auto &current_element : analysis)
    {
        if (!current_element.first.class_name.has_value())
        {
            continue;
        }

        if (result.empty())
        {
            result.push_back({current_element});
            continue;
        }

        std::vector<AnalysisElement> &last_group = result.back();
        const auto &last_element = last_group.back();

        if (st.class_name == current_element.first.class_name)
        {
            last_group.push_back(current_element);
        }
        else
        {
            result.push_back({current_element});
        }
    }

    return result;
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

    std::vector<std::vector<AnalysisElement>> result;

    for (const auto &current_element : analysis)
    {
        if (result.empty())
        {
            result.push_back({current_element});
            continue;
        }

        std::vector<AnalysisElement> &last_group = result.back();
        const auto &last_element = last_group.back();

        if (last_element.first.filename == current_element.first.filename)
        {
            last_group.push_back(current_element);
        }
        else
        {
            result.push_back({current_element});
        }
    }

    return result;
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
    for (const auto &current_element : analysis)
    {
        accumulator.AccumulateNextFunctionResults(current_element.second);
    }
}

}  // namespace analyzer
