cmake_minimum_required(VERSION 3.16)
project(simplesim C)

set(CMAKE_C_STANDARD 99)

# ----------------------------------------------------------------------------
# Compile flags matching the original Makefile defaults
# MFLAGS: from sysprobe (little-endian Linux x86-64)
# FFLAGS: -DDEBUG
# OFLAGS: -O0 -g -Wall
# ----------------------------------------------------------------------------
set(SIMPLESIM_DEFS
    BYTES_LITTLE_ENDIAN
    WORDS_LITTLE_ENDIAN
    FAST_SRL
    FAST_SRA
    GZIP_PATH="/bin/gzip"
    DEBUG
)

set(SIMPLESIM_FLAGS -O0 -g -Wall -w)

# ----------------------------------------------------------------------------
# Target: target-pisa (hard-coded; replaces the old config-pisa symlinks)
# The top-level simplesim dir already has symlinks:
#   config.h  -> target-pisa/config.h
#   machine.h -> target-pisa/pisa.h
#   machine.c -> target-pisa/pisa.c
#   machine.def -> target-pisa/pisa.def
#   loader.c  -> target-pisa/loader.c
#   symbol.c  -> target-pisa/symbol.c
#   syscall.c -> target-pisa/syscall.c
# ----------------------------------------------------------------------------

# Include directories: the simplesim root so that machine.h / config.h resolve
set(SIMPLESIM_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# ----------------------------------------------------------------------------
# libexo static library
# exolex.c is a pre-generated flex output already present in the repo.
# ----------------------------------------------------------------------------
add_library(exo STATIC
    libexo/libexo.c
    libexo/exolex.c
)
target_include_directories(exo PUBLIC ${SIMPLESIM_ROOT})
target_compile_definitions(exo PRIVATE ${SIMPLESIM_DEFS})
target_compile_options(exo PRIVATE ${SIMPLESIM_FLAGS})

# ----------------------------------------------------------------------------
# Common objects shared by all simulators
# (main.c, plus the platform-independent support files)
# sysprobe.c is excluded - it is a standalone host-probe utility.
# ----------------------------------------------------------------------------
set(SIMPLESIM_COMMON_SRCS
    src/main.c
    src/syscall.c
    src/memory.c
    src/regs.c
    src/loader.c
    src/endian.c
    src/dlite.c
    src/symbol.c
    src/eval.c
    src/options.c
    src/stats.c
    src/eio.c
    src/range.c
    src/misc.c
    src/machine.c
    src/eventq.c
    src/resource.c
)

# ----------------------------------------------------------------------------
# sim-bpred
# Extra sources: sim-bpred.c, bpred.c
# ----------------------------------------------------------------------------
add_executable(sim-bpred
    sim-bpred.c
    bpred.c
    ${SIMPLESIM_COMMON_SRCS}
)
target_include_directories(sim-bpred PRIVATE ${SIMPLESIM_ROOT}/inc/)
target_compile_definitions(sim-bpred PRIVATE ${SIMPLESIM_DEFS})
target_compile_options(sim-bpred PRIVATE ${SIMPLESIM_FLAGS})
target_link_libraries(sim-bpred PRIVATE exo m)
