<br>

# Prerequisites

### Install

- **CMake** 3.15 or later - (https://cmake.org/download/)
- **C++ Compiler** with C++17 support:
  - [Visual Studio 2019 or later](https://visualstudio.microsoft.com/downloads/)
  - [MinGW-w64](https://www.mingw-w64.org/downloads/)
- **SFML 2.5** or later - [Download](https://www.sfml-dev.org/download.php)
  - Extract to a location (e.g., `C:/SFML`)
- **OpenGL** support (usually included with graphics drivers)

### Build

1. Configure and build:
   ```bash
   mkdir build
   cd build
   cmake .. -DSFML_ROOT=C:/SFML
   cmake --build . --config Release
   ```

2. Copy SFML DLLs:
   ```powershell
   Copy-Item C:\SFML\bin\sfml-*.dll .\Release\
   ```

### Run

```bash
.\Release\RubikGame.exe
```

### Edit

```bash
cmake --build . --config Release
.\Release\RubikGame.exe
```

<br>

# Victory

Rotating faces until all 6 sides have same color.

<br>

# History

### Invention (1974)
- **Ernő Rubik**: Hungarian sculptor and professor of architecture invented the cube in 1974.
- **Original Purpose**: Created as a teaching tool to help students understand 3D spatial relationships.
- **First Name**: Originally called the "Magic Cube" (Bűvös kocka in Hungarian).
- **Patent**: Rubik applied for a Hungarian patent in 1975.

### Modern (1980s-Present)
- **International Release**: Licensed to Ideal Toy Corp. in 1980, renamed "Rubik's Cube".
- **Worldwide Phenomenon**: Over 100 million cubes sold in the first two years.
- **Algorithms**: Development of advanced solving methods (CFOP, Roux, ZZ, Petrus).
- **Digital Versions**: Transition to computer simulations and virtual reality implementations.

### Logos

- **Permutations**: The cube has 43,252,003,274,489,856,000 (43 quintillion) possible configurations.
- **God's Number**: Proven in 2010 that any scrambled cube can be solved in 20 moves or fewer.

<br>

# Structure

```
.
├── CMakeLists.txt       # Build configuration
├── main.cpp             # Main application and SFML GUI
├── rubik_cube.h/cpp     # Rubik's cube logic and rotation mechanics
├── renderer.h/cpp       # 3D OpenGL rendering system
├── copy_dlls.ps1        # PowerShell script to copy SFML DLLs
├── .gitignore           # Git ignore file
├── R.png                # Project logo/image
└── README.md            # This file
```