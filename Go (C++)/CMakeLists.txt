cmake_minimum_required(VERSION 3.15)
project(GoGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML
# You can specify SFML location with: cmake .. -DSFML_ROOT=C:/SFML
set(SFML_ROOT "" CACHE PATH "Path to SFML installation")
if(SFML_ROOT)
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${SFML_ROOT})
endif()

find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)

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

# SFML targets should be available after find_package
# Set include directories early
if(SFML_INCLUDE_DIR)
    set(SFML_INCLUDE_DIRS ${SFML_INCLUDE_DIR})
elseif(EXISTS "${SFML_ROOT}/include")
    set(SFML_INCLUDE_DIRS "${SFML_ROOT}/include")
endif()

# Source files
set(SOURCES
    main.cpp
    board.cpp
    game.cpp
)

set(HEADERS
    board.h
    game.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Set include directories
if(SFML_INCLUDE_DIRS)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SFML_INCLUDE_DIRS})
endif()

# Link SFML
# Try modern SFML targets first, fallback to legacy if needed
if(TARGET SFML::System)
    target_link_libraries(${PROJECT_NAME} 
        SFML::System
        SFML::Window
        SFML::Graphics
    )
elseif(SFML_LIBRARIES)
    # Fallback for older SFML versions or different configurations
    target_link_libraries(${PROJECT_NAME} 
        ${SFML_LIBRARIES}
    )
    if(SFML_INCLUDE_DIR)
        target_include_directories(${PROJECT_NAME} PRIVATE ${SFML_INCLUDE_DIR})
    endif()
else()
    # Manual linking if targets aren't available
    target_include_directories(${PROJECT_NAME} PRIVATE "${SFML_ROOT}/include")
    if(WIN32)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_link_libraries(${PROJECT_NAME} 
                "${SFML_ROOT}/lib/sfml-graphics-d.lib"
                "${SFML_ROOT}/lib/sfml-window-d.lib"
                "${SFML_ROOT}/lib/sfml-system-d.lib"
            )
        else()
            target_link_libraries(${PROJECT_NAME} 
                "${SFML_ROOT}/lib/sfml-graphics.lib"
                "${SFML_ROOT}/lib/sfml-window.lib"
                "${SFML_ROOT}/lib/sfml-system.lib"
            )
        endif()
    endif()
endif()

# Windows-specific settings
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()

