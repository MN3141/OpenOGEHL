project(OpenOGEHL LANGUAGES C)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -fno-omit-frame-pointer")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(RAYGUI_IMPLEMENTATION)

# Gather source files
file(GLOB SOURCES
    main.c
    ./bp/*.c
    ./common/*.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ../libs/raylib/include
    ../libs/raygui/src/
    ../libs/Unity/src/
    ./bp/
    ./common/
)


# Link raylib and other libraries
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/raylib/lib/libraylib.a
    m
    pthread
)

