cmake_minimum_required(VERSION 3.28.3)
project(analyzer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(GTest REQUIRED)
find_package(range-v3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Сначала настраиваем библиотеки, которые будут подключаться к analyzer
add_subdirectory(src)

# Настраиваем сборку основного приложения analyzer
set(target analyzer)
add_executable(${target} main.cpp)

target_link_libraries(${target} 
    PRIVATE
        metric_accumulator
        metric
        cmd_options
        #range-v3::range-v3
)

# BUILD_INTERFACE автоматически подключает include-пути всех библиотек, указанных в target_link_libraries для analyzer
target_include_directories(${target}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

include(CTest)
enable_testing()

set(test_files_dir ${CMAKE_CURRENT_SOURCE_DIR}/src/metric_impl/tests/files)

add_test(NAME Test1 COMMAND analyzer -f ${test_files_dir}/simple.py)
set_tests_properties(Test1 PROPERTIES
    PASS_REGULAR_EXPRESSION
    "Analysis for every function:;test_simple;Code lines count: 5;Naming style: Snake Case;Parameters count: 0"
)

add_test(NAME Test2 COMMAND analyzer -f ${test_files_dir}/if.py)
set_tests_properties(Test2 PROPERTIES
    PASS_REGULAR_EXPRESSION
    "Analysis for every function:;testIf;Cyclomatic Complexity: 2;Naming style: Camel Case"
)

add_test(NAME Test3 COMMAND analyzer -f ${test_files_dir}/many_parameters.py)
set_tests_properties(Test3 PROPERTIES
    PASS_REGULAR_EXPRESSION
    "Analysis for every function:;__test_multiparameters__;Parameters count: 5"
)

add_test(NAME Test4 COMMAND analyzer -f ${test_files_dir}/missing.py)
set_tests_properties(Test4 PROPERTIES
    WILL_FAIL TRUE
    PASS_REGULAR_EXPRESSION
    "Failed to open source file"
)
