cmake_minimum_required(VERSION 3.10)

project(s2glp_basics)

# ----------------
# Project Settings
# ----------------
# by default the build type is debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# --------------------
# Collect Source Files
# --------------------
# (The commlib files are compiled along with the example)
file(GLOB sources ./*.c ../../src/*.c ../../src/*.h ../../include/*.h)

# ----------------
# Build Executable
# ----------------
add_executable(s2glp_basics ${sources})
target_include_directories(s2glp_basics PRIVATE ../../include)
if(WIN32 OR MINGW OR MSYS)
    target_link_libraries(s2glp_basics PRIVATE setupapi ws2_32)
endif()
