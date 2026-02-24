# AGENTS.md

## Cursor Cloud specific instructions

### Project overview

4D Tesseract Puzzle Visualizer — a C++17 desktop GUI app using SFML 3.0 and OpenGL. Single CMake project, no microservices or databases.

### Prerequisites (already installed in VM snapshot)

- **SFML 3.0.2** built from source and installed to `/usr/local` (distro packages only provide SFML 2.x). Must be compiled with GCC, not Clang.
- **System deps**: `libgl-dev`, `libglu1-mesa-dev`, `libudev-dev`, `libfreetype-dev`, `libx11-dev`, `libxrandr-dev`, `libxcursor-dev`, `libxi-dev`, `mesa-utils`, `xvfb`
- **libstdc++ symlink**: `/usr/lib/x86_64-linux-gnu/libstdc++.so` → GCC's copy (needed because the default compiler is Clang, which can't find the C++ stdlib without it)

### Building

```bash
cd /workspace
mkdir -p build && cd build
cmake .. -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
cmake --build . -j$(nproc)
```

**Important**: Always pass `-DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++` to CMake. The VM default compiler is Clang 18, which cannot find `<chrono>` and other C++ standard library headers without additional configuration.

### Running tests

```bash
./build/test_tesseract
```

The test binary (`test_tesseract`) does NOT link against SFML or OpenGL and runs headlessly. It tests puzzle logic, 4D math, and projection.

### Running the GUI app

```bash
export DISPLAY=:1 LIBGL_ALWAYS_SOFTWARE=1
./build/run
```

- `LIBGL_ALWAYS_SOFTWARE=1` forces Mesa's llvmpipe software renderer (no GPU available in VM).
- The app opens a 1400×1000 window. Use `DISPLAY=:1` for the desktop, or `DISPLAY=:99` with Xvfb for headless.
- Font warnings (`C:/Windows/Fonts/arial.ttf`) are expected on Linux — the font path is hardcoded for Windows. The app still runs, just without text overlays.

### Lint

No linter is configured in this project. The project uses only CMake + GCC; compiler warnings serve as the lint equivalent. You can add `-Wall -Wextra` flags in CMake if needed.
