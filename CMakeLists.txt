cmake_minimum_required(VERSION 3.16)
project(golang_compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Flex and Bison
find_program(FLEX_EXECUTABLE flex REQUIRED)
find_program(BISON_EXECUTABLE bison REQUIRED)

message(STATUS "Flex: ${FLEX_EXECUTABLE}")
message(STATUS "Bison: ${BISON_EXECUTABLE}")

# Set paths
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}")
set(GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")

# Create output directory for generated files
file(MAKE_DIRECTORY "${GENERATED_DIR}")

# Bison generates both .cpp and .hpp files
set(GENERATED_PARSER_CPP "${GENERATED_DIR}/golang_parser.cpp")
set(GENERATED_PARSER_HPP "${GENERATED_DIR}/golang_parser.hpp")

# Flex generates .cpp file
set(GENERATED_LEXER_CPP "${GENERATED_DIR}/lex.yy.cpp")

# Bison command - use -o with just filename, let working directory handle the path
add_custom_command(
        OUTPUT "${GENERATED_PARSER_CPP}" "${GENERATED_PARSER_HPP}"
        COMMAND "${BISON_EXECUTABLE}"
        ARGS -Wall -d -o golang_parser.cpp "${SOURCE_DIR}/golang_parser_with_tree_code.y"
        DEPENDS "${SOURCE_DIR}/golang_parser_with_tree_code.y"
        WORKING_DIRECTORY "${GENERATED_DIR}"
        COMMENT "Generating parser with Bison"
        VERBATIM
)

# Flex command
add_custom_command(
        OUTPUT "${GENERATED_LEXER_CPP}"
        COMMAND "${FLEX_EXECUTABLE}"
        ARGS -o "${GENERATED_DIR}/lex.yy.cpp" "${SOURCE_DIR}/golang_lexer.l"
        DEPENDS "${SOURCE_DIR}/golang_lexer.l" "${GENERATED_PARSER_HPP}"
        WORKING_DIRECTORY "${GENERATED_DIR}"
        COMMENT "Generating lexer with Flex"
        VERBATIM
)

# Create executable
add_executable(golang_compiler
        main.cpp
        classes.cpp
        ${GENERATED_LEXER_CPP}
        ${GENERATED_PARSER_CPP}
)

# Set language for generated files
set_source_files_properties(${GENERATED_LEXER_CPP} PROPERTIES LANGUAGE CXX)
set_source_files_properties(${GENERATED_PARSER_CPP} PROPERTIES LANGUAGE CXX)

# Include directories
target_include_directories(golang_compiler PRIVATE
        "${SOURCE_DIR}"
        "${GENERATED_DIR}"
)
