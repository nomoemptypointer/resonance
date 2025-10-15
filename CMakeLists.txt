cmake_minimum_required(VERSION 3.15)
project(resonance LANGUAGES CXX)
include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(resonance SHARED
        src/Resonance.cpp
        src/Sound.cpp
)

target_compile_definitions(resonance PRIVATE RESONANCE_EXPORTS)

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "Resonance can only be built with GCC or MinGW.")
endif()

# Apply size optimizations
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Applying size optimizations for library")
    target_compile_options(resonance PRIVATE
            -Os
            -ffunction-sections
            -fdata-sections
    )
else()
    message(STATUS "Debug build: optimizations disabled")
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