<img width="694" height="495" alt="T" src="https://github.com/user-attachments/assets/c5b8a018-ed4e-40d0-9447-5cb5f1490c7c" />

# Prior

## Install

- **CMake** 3.15 or later — https://cmake.org/download/
- **C++17 compiler** — https://visualstudio.microsoft.com/downloads/
- **SFML 3.0** or later — https://www.sfml-dev.org/download.php
  - Extract to a location (e.g., `C:/SFML`)
- **OpenGL** support (included with graphics drivers)

## Build

   ```powershell
   mkdir build
   cd build
   cmake .. -DSFML_ROOT=C:/SFML #Tell CMake, where SFML is installed
   cmake --build . --config Release #creates Release\ and run.exe
   copy C:\SFML\SFML-3.0.2\bin\*.dll .\Release\ #copy DLLs into created folder
   ```
   (Copy SFML DLLs once per build folder.)

## Run

```powershell
.\Release\run.exe
```


# Function

## 4D Cube (Tesseract)

```text
┌─────────────────────────────────────────────────────────────────┐
│ TESSERACT GEOMETRY                                              │
│ • 4D hypercube: all points (x,y,z,w) with each coord ∈ {-1,+1}  │
│ • 16 vertices, 32 edges, 24 square faces, 8 cubic cells         │
│ • Analog: 1D→2D (line→square), 2D→3D (square→cube), 3D→4D       │
└─────────────────────────────────────────────────────────────────┘
                                    ▼
┌─────────────────────────────────────────────────────────────────┐
│ 4D ROTATION                                                     │
│ • 6 planes: XY, XZ, XW, YZ, YW, ZW (choose 2 axes, rotate)      │
│ • rotate4D(plane, θ) = Givens rotation in that plane            │
│ • Layer = which w-slice (0 or 1) is fixed during rotation       │
└─────────────────────────────────────────────────────────────────┘
                                    ▼
┌─────────────────────────────────────────────────────────────────┐
│ 4D → 3D PROJECTION                                              │
│ • Perspective: scale = wDistance / (wDistance + w)              │
│ • (x,y,z,w) → (x·scale, y·scale, z·scale)                       │
│ • Points farther in +w appear smaller (depth cue)               │
└─────────────────────────────────────────────────────────────────┘
                                    ▼
┌─────────────────────────────────────────────────────────────────┐
│ 3D RENDERING                                                    │
│ • 32 edges, 16 vertices (colored stickers)                      │
│ • 3D camera (drag/zoom) on projected result                     │
└─────────────────────────────────────────────────────────────────┘
```

# History

```text
┌─────────────────────────────────────────────────────────────────┐
│ 1852 – SCHLÄFLI                                                 │
│ • Theorie der vielfachen Kontinuität                            │
│ • 6 convex regular 4-polytopes; tesseract {4,3,3}               │
│ • Schläfli symbol notation                                      │
└─────────────────────────────────────────────────────────────────┘
                                    ▼
┌─────────────────────────────────────────────────────────────────┐
│ 1880–1884 – POLYTOPE RUSH / POPULARIZATION                      │
│ • Stringham (1880): American J. Math, 4D illustrations          │
│ • Hess (1883): complete list of regular 4-polytopes             │
│ • Abbott (1884): Flatland – 4D in popular culture               │
└─────────────────────────────────────────────────────────────────┘
                                    ▼
┌─────────────────────────────────────────────────────────────────┐
│ 1904–1913 – PHYSICS                                             │
│ • Minkowski (1904): spacetime as 4D manifold                    │
│ • Einstein (1905): special relativity                           │
│ • Bragdon (1913): A Primer of Higher Space, "hypercube" term    │
└─────────────────────────────────────────────────────────────────┘
                                    ▼
┌─────────────────────────────────────────────────────────────────┐
│ 1974–1988 – PUZZLES                                             │
│ • Rubik's Cube (1974): 3D twisty puzzle phenomenon              │
│ • Magic Cube 4D (1988): Hatch, Green – first 4D digital puzzle  │
└─────────────────────────────────────────────────────────────────┘
                                    ▼
┌─────────────────────────────────────────────────────────────────┐
│ 2000s–PRESENT                                                   │
│ • MC4D applet (2002), 2×2×2×2–6×6×6×6 sizes                     │
│ • Open-source 4D puzzle community, Discord, solvers             │
└─────────────────────────────────────────────────────────────────┘
```

# Structure

```text
├── CMakeLists.txt       # Build configuration              (Backend) (Config)
├── copy_dlls.ps1        # Copy SFML DLLs to build output   (Backend) (Config)
├── .gitignore           # Git ignore patterns              (Config)
├── main.cpp             # SFML window, game loop, input    (Frontend) (Source / Script)
├── tesseract_model.h    # 4D puzzle state and moves        (Backend) (Source / Header)
├── tesseract_model.cpp  # Tesseract logic                  (Backend) (Source / Library)
├── rubik_cube.h         # 3×3×3 inner cube                 (Backend) (Source / Header)
├── rubik_cube.cpp       # Rubik logic                      (Backend) (Source / Library)
├── math_4d.h            # Vec4, Mat4x4, 4D rotations       (Backend) (Source / Header)
├── math_4d.cpp          # 4D math implementation           (Backend) (Source / Library)
├── projection_4d.h      # 4D→3D projection                 (Backend) (Source / Header)
├── projection_4d.cpp    # Projection implementation        (Backend) (Source / Library)
├── renderer.h           # 4D renderer interface            (Frontend) (Source / Header)
├── renderer.cpp         # OpenGL 4D rendering              (Frontend) (Source / Library)
├── test_tesseract.cpp   # Smoke tests for puzzle logic     (Backend) (Test)
└── README.md            # This file
```
