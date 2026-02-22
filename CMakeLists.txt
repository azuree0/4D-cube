cmake_minimum_required(VERSION 3.15)
project(Tesseract)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML
set(SFML_ROOT "" CACHE PATH "Path to SFML installation")
if(SFML_ROOT)
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${SFML_ROOT})
endif()

find_package(SFML 3.0 COMPONENTS System Window Graphics REQUIRED)

if(NOT SFML_FOUND)
    message(FATAL_ERROR 
        "SFML not found!\n"
        "Please install SFML 2.5 or later and either:\n"
        "  1. Set SFML_ROOT environment variable to your SFML installation\n"
        "  2. Run: cmake .. -DSFML_ROOT=C:/SFML (adjust path as needed)\n"
        "  3. Or install SFML via vcpkg: vcpkg install sfml\n"
        "\n"
        "Download SFML from: https://www.sfml-dev.org/download.php"
    )
endif()

# OpenGL
find_package(OpenGL QUIET)
if(NOT OPENGL_FOUND)
    # Try alternative method for OpenGL
    find_path(OPENGL_INCLUDE_DIR GL/gl.h
        PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        "C:/Program Files/Microsoft SDKs/Windows/*/Include"
    )
    
    if(WIN32)
        find_library(OPENGL_LIBRARY opengl32
            PATHS
            "C:/Program Files/Microsoft SDKs/Windows/*/Lib"
        )
        if(OPENGL_LIBRARY)
            set(OPENGL_LIBRARIES ${OPENGL_LIBRARY})
        endif()
    else()
        find_library(OPENGL_LIBRARY GL
            PATHS
            /usr/lib
            /usr/local/lib
            /opt/local/lib
        )
        if(OPENGL_LIBRARY)
            set(OPENGL_LIBRARIES ${OPENGL_LIBRARY})
        endif()
    endif()
    
    if(OPENGL_INCLUDE_DIR)
        set(OPENGL_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
    endif()
endif()

if(NOT OPENGL_LIBRARIES)
    message(WARNING "OpenGL not found, but continuing. The application may not work correctly.")
    set(OPENGL_LIBRARIES "")
endif()

# Executable (output binary: run)
set(SOURCES
    main.cpp
    tesseract_model.cpp
    rubik_cube.cpp
    math_4d.cpp
    projection_4d.cpp
    renderer.cpp
)

set(HEADERS
    tesseract_model.h
    rubik_cube.h
    math_4d.h
    projection_4d.h
    renderer.h
)

add_executable(run ${SOURCES} ${HEADERS})

# Smoke tests
add_executable(test_tesseract
    test_tesseract.cpp
    tesseract_model.cpp
    math_4d.cpp
    projection_4d.cpp
)
target_include_directories(test_tesseract PRIVATE ${CMAKE_SOURCE_DIR})

# Set include directories
if(SFML_INCLUDE_DIRS)
    target_include_directories(run PRIVATE ${SFML_INCLUDE_DIRS})
endif()

if(OPENGL_INCLUDE_DIRS)
    target_include_directories(run PRIVATE ${OPENGL_INCLUDE_DIRS})
endif()

# Link SFML
if(TARGET SFML::System)
    target_link_libraries(run
        SFML::System
        SFML::Window
        SFML::Graphics
    )
elseif(SFML_LIBRARIES)
    target_link_libraries(run
        ${SFML_LIBRARIES}
    )
    if(SFML_INCLUDE_DIR)
        target_include_directories(run PRIVATE ${SFML_INCLUDE_DIR})
    endif()
elseif(SFML_ROOT)
    # Manual linking if targets aren't available but SFML_ROOT is set
    target_include_directories(run PRIVATE "${SFML_ROOT}/include")
    if(WIN32)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_link_libraries(run
                "${SFML_ROOT}/lib/sfml-graphics-d.lib"
                "${SFML_ROOT}/lib/sfml-window-d.lib"
                "${SFML_ROOT}/lib/sfml-system-d.lib"
            )
        else()
            target_link_libraries(run
                "${SFML_ROOT}/lib/sfml-graphics.lib"
                "${SFML_ROOT}/lib/sfml-window.lib"
                "${SFML_ROOT}/lib/sfml-system.lib"
            )
        endif()
    endif()
endif()

# Link OpenGL
if(OPENGL_LIBRARIES)
    target_link_libraries(run ${OPENGL_LIBRARIES})
endif()

# Windows-specific settings
if(WIN32)
    set_target_properties(run PROPERTIES WIN32_EXECUTABLE FALSE)
    set_target_properties(test_tesseract PROPERTIES WIN32_EXECUTABLE FALSE)
endif()

