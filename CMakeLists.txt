cmake_minimum_required(VERSION 3.15)
project(resonance LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(resonance STATIC
        src/resonance.cpp
)

add_executable(test_app
        test/main.cpp
)

target_link_libraries(test_app PRIVATE resonance)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)
if(PORTAUDIO_FOUND)
    target_include_directories(test_app PRIVATE ${PORTAUDIO_INCLUDE_DIRS})
    target_link_libraries(test_app PRIVATE ${PORTAUDIO_LIBRARIES})
endif()

# Add a "run_test" custom target
add_custom_target(run_test
        COMMAND test_app
        DEPENDS test_app
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
