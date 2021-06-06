cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

project(simple-cpp-option-parser)


# This logger is written considering C++11
set (CMAKE_CXX_STANDARD 11)

# If Clang (Apple) disable extra-warnings about format security
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # using regular Clang or AppleClang
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-format-security")
endif()


# Options
add_definitions( -D CMDLINEPARSER_DEFAULT_FASCIST_MODE=1 )


# Include the header folder
include_directories(./include)


# Executable example
message("Setting up executable: OptionParserExample")
add_executable( OptionParserExample example/main.cpp )


# Extension: yaml-cpp
find_package( yaml-cpp )
if( yaml-cpp_FOUND )
    message("yaml-cpp has been found! Compiling with extra features")
    message("  -> include directory: ${YAML_CPP_INCLUDE_DIR}")
    message("  -> lib name: ${YAML_CPP_LIBRARIES}")
    include_directories( ${YAML_CPP_INCLUDE_DIR} )
    target_link_libraries( OptionParserExample ${YAML_CPP_LIBRARIES} )
    add_definitions( -D CMDLINEPARSER_YAML_CPP_ENABLED=1 )
endif()

