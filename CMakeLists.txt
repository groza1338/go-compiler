cmake_minimum_required(VERSION 3.16)
project(lua_compiler)

set(CMAKE_CXX_STANDARD 11)

# Add flex dependence to project
find_program(FLEX_EXECUTABLE flex)
if(NOT FLEX_EXECUTABLE)
    message(FATAL_ERROR "Flex not found! Install flex and try again.")
endif()

set(GENERATED_LEXER_FILE "${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cpp")

# Generation flex source file
add_custom_command(
        OUTPUT ${GENERATED_LEXER_FILE}
        COMMAND ${FLEX_EXECUTABLE} --c++ -o ${GENERATED_LEXER_FILE} golang_lexer.l
        DEPENDS golang_lexer.l
        COMMENT "Generating C++ lexer with Flex"
)

# Add files to executable target
add_executable(lua_compiler
        main.cpp
        ${GENERATED_LEXER_FILE}
)

set_source_files_properties(${GENERATED_LEXER_FILE} PROPERTIES LANGUAGE CXX)


# Add link to flex library
target_link_libraries(lua_compiler PRIVATE fl)
