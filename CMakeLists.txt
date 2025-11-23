cmake_minimum_required(VERSION 3.14)
project(main)

option(UNIT_TESTS "Create config for unit tests" 0)

if (NOT ${UNIT_TESTS})
    add_subdirectory(./src/)
else()
    add_subdirectory(./test/)
endif()
