cmake_minimum_required(VERSION 3.15)
project(quantum_noise_sim)
include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

execute_process(
    COMMAND python -c "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_VARIABLE pybind11_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG        3.4.0
    GIT_SHALLOW    TRUE
)
set(EIGEN_BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(eigen)

find_package(Eigen3 REQUIRED)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)

include_directories(src)

# C++ core library
add_library(qnoise_core
    src/core/DensityMatrix/density_matrix.cpp
    src/core/stateVector/stateVector.cpp
    src/core/channels/AmplitudeDamping/amplitude_damping.cpp
    src/core/channels/BitPhaseFlips/bit_phase_flips.cpp
    src/core/channels/DepolarizingNoise/depolarizing_noise.cpp
    src/core/channels/PhaseDamping/phase_damping.cpp
    src/core/QuantumCircuit/QuantumCircuit.cpp
)

target_link_libraries(qnoise_core Eigen3::Eigen)

# Python bindings
pybind11_add_module(qnoise src/bindings/pybind_module.cpp)
target_link_libraries(qnoise PRIVATE qnoise_core Eigen3::Eigen)

if(APPLE)
    execute_process(
        COMMAND xcrun --show-sdk-path
        OUTPUT_VARIABLE CMAKE_OSX_SYSROOT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

option(BUILD_TESTS "Build C++ tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)

    set(TEST_FILES test_measurement test_state_vector test_density_matrix test_noise_channels)

    foreach(test ${TEST_FILES})
        add_executable(${test} tests/cpp/${test}.cpp)
        target_link_libraries(${test} GTest::gtest GTest::gtest_main qnoise_core)
        add_test(NAME ${test} COMMAND ${test})
    endforeach()
endif()

if(NOT SKBUILD)
    set_target_properties(qnoise PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/python/qnoise
    )
endif()