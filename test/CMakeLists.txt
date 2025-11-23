project(OpenOGEHL_utest LANGUAGES C)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -fno-omit-frame-pointer")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(RAYGUI_IMPLEMENTATION)
add_compile_definitions(UNIT_TESTS ON)

# Gather source files
file(GLOB SOURCES
    ./main.c
    ../src/bp/*.c
    ../src/common/*.c
    ../libs/Unity/src/*.c
    ./bp/*.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ../libs/raylib/include
    ../libs/raygui/src/
    ../libs/Unity/src/
    ../src/bp/
    ../src/common/
    ./bp/
)


# Link raylib and other libraries
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/raylib/lib/libraylib.a
    m
    pthread
)

