##  Directory structure

Go (C++)/
├── main.cpp          # Main application and SFML GUI
├── board.h/cpp       # Board logic and Go rules
├── game.h/cpp        # Game state management
├── CMakeLists.txt    # Build configuration
├── .gitignore        # Git ignore file
└── README.md         # This file

## Prerequisites

Before building, ensure you have:

- **C++ Compiler** with C++17 support:
  - Windows: Visual Studio 2019 or later, or MinGW-w64
  - Linux: GCC 7+ or Clang 5+
  - macOS: Xcode 10+ or Clang 5+

- **CMake** 3.15 or later
  - Download from [cmake.org](https://cmake.org/download/)

- **SFML 2.5** or later
  - **Windows**: Download from [sfml-dev.org](https://www.sfml-dev.org/download.php)
    - Extract to a location (e.g., `C:\SFML`)
    - Set `SFML_ROOT` environment variable or specify in CMake
  - **Linux**: Install via package manager
    ```bash
    # Ubuntu/Debian
    sudo apt-get install libsfml-dev
    
    # Fedora
    sudo dnf install SFML-devel
    ```
  - **macOS**: Install via Homebrew
    ```bash
    brew install sfml
    ```

## Building the Project

### Windows

**Using Visual Studio (Recommended)**:
```powershell
mkdir build
cd build
cmake .. -DSFML_ROOT=C:/SFML       # Adjust path to your SFML installation
cmake --build . --config Release
```

**Using MinGW**:
```powershell
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DSFML_ROOT=C:/SFML
cmake --build .
```

### Linux/Mac

```bash
mkdir build
cd build
cmake ..
make -j4
```

**Note:** The first build may take several minutes as it downloads and compiles dependencies.

## Running the Game

After building, run the executable:

- **Windows**: `build/Release/GoGame.exe` or `build/Debug/GoGame.exe`

- **Linux/macOS**: `./build/GoGame`