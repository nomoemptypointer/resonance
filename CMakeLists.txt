cmake_minimum_required(VERSION 3.15)
project(resonance LANGUAGES CXX)
include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(resonance SHARED
        src/Resonance.cpp
        src/AudioStream.h
        include/Types.h
)

target_compile_definitions(resonance PRIVATE RESONANCE_EXPORTS)

# Compiler and linker optimizations for size
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Applying size optimizations for library")

    # Optimize for size, remove unused code sections
    target_compile_options(resonance PRIVATE
            -Os
            # -O1 → basic optimization
            # -O2 → good general optimization
            # -O3 → aggressive optimization (may increase code size in some cases)
            # -Os → optimize for size (usually the best for DLLs)
            -ffunction-sections
            -fdata-sections
    )
else()
    message(FATAL_ERROR "Resonance can only be built with GCC or MinGW.")
endif()

CPMAddPackage(
        NAME SDL3
        GITHUB_REPOSITORY libsdl-org/SDL
        GIT_TAG release-3.2.0 # stable release
        OPTIONS
        "SDL_TEST OFF"
        "SDL_STATIC ON"
        "SDL_SHARED ON"
)

add_executable(test_app
        test/main.cpp
)

target_link_libraries(test_app PRIVATE
        resonance
        SDL3::SDL3
)

if(MINGW)
    target_link_options(test_app PRIVATE -static-libgcc -static-libstdc++)
endif()