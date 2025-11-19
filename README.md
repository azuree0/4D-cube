## Directory structure

Rubik (C++)/
├── main.cpp          # Main application and SFML GUI
├── rubik_cube.h/cpp  # Rubik's cube logic and rotation mechanics
├── renderer.h/cpp    # 3D OpenGL rendering system
├── CMakeLists.txt    # Build configuration
├── .gitignore        # Git ignore file
└── README.md         # This file

### Prerequisites

- CMake 3.15 or later
- C++17 compatible compiler
- SFML 2.5 or later
- OpenGL support

### Windows Build Instructions

1. Install SFML:
   - Download from https://www.sfml-dev.org/download.php
   - Extract to a location (e.g., `C:/SFML`)

2. Configure and build:
   ```bash
   mkdir build
   cd build
   cmake .. -DSFML_ROOT=C:/SFML
   cmake --build . --config Release
   ```

3. Copy SFML DLLs (if not already done):
   ```powershell
   Copy-Item C:\SFML\bin\sfml-*.dll build\Release\
   ```

4. Run:
   ```bash
   cd build\Release
   .\RubikGame.exe
   ```
   Or from the project root:
   ```bash
   .\build\Release\RubikGame.exe
   ```

### Alternative: Using vcpkg

If you have vcpkg installed:
```bash
vcpkg install sfml
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```