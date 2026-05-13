# OpenOGEHL

OpenOGEHL is an implementation of the **OGEHL (O-GEometric History Length) branch predictor**, built as a faculty project. The application simulates the predictor using configurable parameters and provides an interactive GUI to visualize and control the simulation. Branch trace files (`.TRA`) are fed as input, and the predictor's accuracy can be evaluated across different table counts, table sizes, and counter lengths.

---

## How to Build

Requires **CMake ≥ 3.14**, a C compiler.

A top-level `Makefile` wraps the common CMake workflows:

### Main Application

```bash
# For first time use after git clone
git submodule init
git submodule update

# Configure
make all

# Build
make b

# Run
make r
```

The binary is produced at `builds/app/src/OpenOGEHL`.

### Unit Tests

```bash
# Configure
make t

# Build
make tb

# Run
make tr
```

The test binary is produced at `builds/utest/test/OpenOGEHL_utest`.

### Manual CMake

```bash
# App
cmake -S . -B builds/app -G "Unix Makefiles"
cmake --build builds/app

# Tests
cmake -S . -B builds/utest -DUNIT_TESTS=1 -G "Unix Makefiles"
cmake --build builds/utest
```

---

## SimpleScalar Simulator Integration

The `simplesim/` directory contains a modified [SimpleScalar](http://www.simplescalar.com/) simulator with the OGEHL predictor integrated into `bpred.c`. It is used to evaluate prediction accuracy against real SPEC benchmark traces.

All build and run operations go through the `gen` script:

```bash
cd simplesim

# Configure (cmake)
./gen

# Build (parallel, -j12)
./gen b

# Run a benchmark with the default config  (num_tables=7, table_size=2048, counter_len=4)
./gen applu
./gen cc1
./gen mgrid
./gen compress
./gen ijpeg

# Run a specific named configuration
./gen applu c1        # min tables, small table, short counters
./gen applu c9        # max tables, larger table, long counters

# Run all 9 configurations for a benchmark
./gen applu all
```

### Named Configurations

| Config | num_tables | table_size | counter_len | Notes |
|--------|-----------|------------|-------------|-------|
| `r` (default) | 7 | 2048 | 4 | Baseline |
| `c1` | 4 | 512 | 3 | Minimum everything |
| `c2` | 4 | 2048 | 4 | Min tables, default size |
| `c3` | 7 | 512 | 3 | Default tables, small |
| `c4` | 7 | 2048 | 4 | Baseline (same as `r`) |
| `c5` | 7 | 2048 | 5 | Longer counters |
| `c6` | 7 | 8192 | 4 | Larger table |
| `c7` | 12 | 2048 | 4 | Max tables |
| `c8` | 12 | 8192 | 4 | Max tables + larger table |
| `c9` | 12 | 8192 | 5 | Maximum everything |

Results are saved to `simplesim/results/<benchmark>/<config>_simout.res`.

### Benchmarks

| Benchmark | Binary | Description |
|-----------|--------|-------------|
| `applu` | `applu.ss` | NASA parallel CFD kernel |
| `cc1` | `cc1.ss` | GCC compiler front-end |
| `mgrid` | `mgrid.ss` | Multigrid solver |
| `compress` | `compress95.ss` | Lempel-Ziv data compression |
| `ijpeg` | `ijpeg.ss` | JPEG encoder/decoder |

---

## Repository Overview

```
OpenOGEHL/
├── CMakeLists.txt          # Top-level CMake; switches between app and utest targets
├── Makefile                # Convenience wrapper for common CMake commands
│
├── src/                    # Main application source
│   ├── main.c              # Entry point: initialises UI and spawns worker tasks
│   ├── CMakeLists.txt
│   │
│   ├── bp/                 # Branch predictor (OGEHL core)
│   │   ├── bp.h / bp.c     # Public API: BP_Init, BP_GetPrediction, BP_Update
│   │   ├── bp_defines.h    # Compile-time constants (table limits, defaults, thresholds)
│   │   ├── bp_types.h      # Primitive type aliases (counters, GHR, tags)
│   │   └── utils.h / utils.c
│   │
│   ├── ui/                 # GUI layer (raylib + raygui)
│   │   ├── ui.h / ui.c     # Window initialisation and main render loop
│   │   ├── style_dark.h    # Dark theme style data
│   │   └── gui_window_file_dialog.h  # File-picker widget
│   │
│   ├── common/             # Shared threading and task infrastructure
│   │   ├── task.h / task.c         # Task creation (CreateTasks)
│   │   └── thread_com.h / thread_com.c  # Inter-thread communication helpers
│   │
│   └── template/           # Boilerplate module skeleton
│       └── template.h / template.c
│
├── test/                   # Unit test suite (Unity framework)
│   ├── main.c              # Test runner entry point
│   ├── CMakeLists.txt
│   └── bp/
│       ├── bp_test.h
│       └── bp_test.c       # Unit tests for the branch predictor module
│
├── input/                  # Branch trace files used as predictor input
│   ├── FBUBBLE.TRA
│   ├── FMATRIX.TRA
│   ├── FPERM.TRA
│   ├── FPUZZLE.TRA
│   ├── FQUEENS.TRA
│   ├── FSORT.TRA
│   ├── FTOWER.TRA
│   └── FTREE.TRA
│
├── libs/                   # Vendored third-party libraries
│   ├── raylib/             # Rendering & windowing library
│   ├── raygui/             # Immediate-mode GUI widgets built on raylib
│   └── Unity/              # Lightweight C unit-testing framework
│
├── simplesim/              # SimpleScalar-based branch prediction simulator
│   ├── gen                 # ⭐ Build & run script — primary way to use the simulator
│   ├── sim-bpred.c         # Simulator main
│   ├── bpred.c             # Simulator predictor implementations (includes OGEHL)
│   ├── src/                # Loader, machine model, syscall emulation, etc.
│   ├── inc/                # Simulator headers
│   ├── inputs/             # SPEC benchmark inputs
│   │   ├── ss_files/       # Compiled SimpleScalar binaries (applu, cc1, mgrid, compress95, ijpeg)
│   │   └── in_files/       # Corresponding stdin input files for each benchmark
│   ├── results/            # Simulation output files (per benchmark, per config)
│   └── build/              # Simulator build artefacts (generated by gen)
│
├── builds/                 # CMake out-of-source build directories (generated)
│   ├── app/                # Main app build
│   └── utest/              # Unit test build
│
└── doc/                    # LaTeX project documentation / thesis
```

---

## Key Parameters (OGEHL)

| Parameter | Default | Range | Description |
|-----------|---------|-------|-------------|
| Number of tables | 7 | 4 – 12 | Controls prediction accuracy vs. area |
| Table size | 2048 entries | 32 KB – 1 MB total | Entries per history table |
| Counter length | 4 bits | 3 – 5 bits | Saturating counter width |

