cmake_minimum_required(VERSION 3.14)
project(OpenOGEHL LANGUAGES C)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_BUILD_TYPE Debug)
# Ensure debug builds include symbols and disable optimizations for easier debugging
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -fno-omit-frame-pointer")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(RAYGUI_IMPLEMENTATION)

# Gather source files
file(GLOB SOURCES
    main.c
    ./src/bp/*.c
    ./src/common/*.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ./libs/raylib/include
    ./libs/raygui/src/
    ./src/bp/
    ./src/common/
)

# Link raylib and other libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/libs/raylib/lib/libraylib.a m pthread)
