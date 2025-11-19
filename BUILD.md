# Build Instructions

Detailed guide for building the 3D Rubik's Cube game on different platforms.

## Directory Structure

```
Rubik (C++)/
├── main.cpp          # Main application and SFML GUI
├── rubik_cube.h/cpp  # Rubik's cube logic and rotation mechanics
├── renderer.h/cpp    # 3D OpenGL rendering system
├── CMakeLists.txt    # Build configuration
├── .gitignore        # Git ignore file
└── README.md         # Project overview
```

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
    
    # Arch Linux
    sudo pacman -S sfml
    ```
  - **macOS**: Install via Homebrew
    ```bash
    brew install sfml
    ```

- **OpenGL** support (usually included with graphics drivers)
  - OpenGL 2.1 or later required
  - Most modern systems have this by default

## Building the Project

### Windows

#### Using Visual Studio (Recommended)

1. Open PowerShell or Command Prompt in the project directory

2. Create build directory and configure:
   ```powershell
   mkdir build
   cd build
   cmake .. -DSFML_ROOT=C:/SFML
   ```
   Adjust the `SFML_ROOT` path to match your SFML installation location.

3. Build the project:
   ```powershell
   cmake --build . --config Release
   ```

4. The executable will be in `build/Release/RubikGame.exe`

#### Using MinGW

1. Open PowerShell or Command Prompt in the project directory

2. Create build directory and configure:
   ```powershell
   mkdir build
   cd build
   cmake .. -G "MinGW Makefiles" -DSFML_ROOT=C:/SFML
   ```

3. Build the project:
   ```powershell
   cmake --build .
   ```

4. The executable will be in `build/RubikGame.exe`

#### Using vcpkg

If you have vcpkg installed:

1. Install SFML via vcpkg:
   ```powershell
   vcpkg install sfml
   ```

2. Configure CMake with vcpkg toolchain:
   ```powershell
   mkdir build
   cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake
   ```

3. Build:
   ```powershell
   cmake --build . --config Release
   ```

### Linux

1. Install dependencies (if not already installed):
   ```bash
   # Ubuntu/Debian
   sudo apt-get install build-essential cmake libsfml-dev

   # Fedora
   sudo dnf install gcc-c++ cmake SFML-devel

   # Arch Linux
   sudo pacman -S base-devel cmake sfml
   ```

2. Build the project:
   ```bash
   mkdir build
   cd build
   cmake ..
   make -j4
   ```

3. The executable will be in `build/RubikGame`

### macOS

1. Install dependencies via Homebrew:
   ```bash
   brew install cmake sfml
   ```

2. Build the project:
   ```bash
   mkdir build
   cd build
   cmake ..
   make -j4
   ```

3. The executable will be in `build/RubikGame`

## Running the Game

After building, run the executable:

- **Windows**: 
  ```powershell
  cd build
  .\Release\RubikGame.exe
  ```
  Or double-click `RubikGame.exe` in the `build/Release/` folder

- **Linux/macOS**: 
  ```bash
  cd build
  ./RubikGame
  ```

**Note:** On Windows, you may need to copy SFML DLLs to the same directory as the executable:
- Copy `sfml-graphics-2.dll`, `sfml-window-2.dll`, and `sfml-system-2.dll` from your SFML installation's `bin` folder to `build/Release/`

## Troubleshooting

### CMake can't find SFML

**Windows:**
- Set the `SFML_ROOT` environment variable to your SFML installation directory
- Or specify it during CMake configuration: `cmake .. -DSFML_ROOT=C:/SFML`
- Make sure the path uses forward slashes (`/`) or escaped backslashes (`\\`)

**Linux/macOS:**
- Ensure SFML is installed via package manager
- If installed manually, set `SFML_ROOT` environment variable
- Check that SFML libraries are in standard system paths (`/usr/lib`, `/usr/local/lib`)

### Build fails with compiler errors

- Ensure you have C++17 support enabled
- Check your compiler version:
  - GCC: `gcc --version` (need 7+)
  - Clang: `clang --version` (need 5+)
  - MSVC: Check Visual Studio version (need 2019+)

### Linker errors

- Verify SFML is properly linked
- On Windows, ensure SFML DLLs are accessible
- On Linux, check that SFML libraries are installed: `ldconfig -p | grep sfml`

### OpenGL errors

- Ensure your graphics drivers are up to date
- Check OpenGL version: `glxinfo | grep "OpenGL version"` (Linux) or use OpenGL Extensions Viewer (Windows)
- Minimum required: OpenGL 2.1

### Font not loading

- The game tries to load Arial or Calibri from Windows fonts directory
- On Linux/macOS, you may need to adjust the font path in `main.cpp`
- The game will still work without fonts, but text may not display correctly
- To fix: Edit `main.cpp` and change the font path to a system font on your platform

### Game doesn't respond to input

- Check that the executable is running properly
- Ensure SFML libraries are in the same directory as the executable (Windows) or in system library path (Linux/macOS)
- Verify OpenGL context is properly initialized (check for OpenGL errors in console)
- Try running from command line to see error messages

### 3D rendering issues

- Ensure your graphics drivers are up to date
- Check that OpenGL 2.1 or later is supported
- On some systems, you may need to install additional OpenGL libraries:
  - Linux: `sudo apt-get install libgl1-mesa-dev` (Ubuntu/Debian)
  - Some systems may require `libglu1-mesa-dev` for GLU functions

### Performance issues

- The game runs at 60 FPS by default
- If experiencing lag, check:
  - Graphics driver updates
  - Background processes consuming resources
  - Try reducing the framerate limit in `main.cpp` (line with `setFramerateLimit`)

## Development

### Debug Build

To build in Debug mode:

**Windows (Visual Studio):**
```powershell
cmake --build . --config Debug
```

**Linux/macOS:**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
```

### Cleaning Build

To clean the build directory:

```bash
# Remove build directory
rm -rf build  # Linux/macOS
rmdir /s build  # Windows PowerShell
```

### Rebuilding

To rebuild from scratch:

1. Delete the `build` directory
2. Follow the build instructions again

## Additional Notes

- The project uses fixed-function OpenGL (legacy OpenGL 2.1)
- SFML handles window creation and OpenGL context management
- All 3D rendering is done with immediate mode OpenGL
- The cube state is stored as 6 faces with 3x3 grids

