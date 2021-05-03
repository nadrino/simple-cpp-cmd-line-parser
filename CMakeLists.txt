cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

project(simple-cpp-option-parser)


# This logger is written considering C++11
set (CMAKE_CXX_STANDARD 11)

# If Clang (Apple) disable extra-warnings about format security
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # using regular Clang or AppleClang
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-format-security")
endif()

# Include the header folder
include_directories(./include)

add_executable(OptionParserExample example/main.cpp )
