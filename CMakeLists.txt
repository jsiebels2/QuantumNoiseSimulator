cmake_minimum_required(VERSION 3.15)
project(quantum_noise_sim)

set(CMAKE_CXX_STANDARD 17)

find_package(Eigen3 REQUIRED)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)

include_directories(src)

# C++ core library
add_library(qnoise_core
    src/core/density_matrix.cpp
    src/core/channel.cpp
    src/circuit/circuit.cpp
    src/stateVector/stateVector.cpp
)

target_link_libraries(qnoise_core Eigen3::Eigen)

# Python bindings
pybind11_add_module(qnoise_cpp src/bindings/pybind_module.cpp)
target_link_libraries(qnoise_cpp PRIVATE qnoise_core Eigen3::Eigen)

if(APPLE)
    execute_process(
        COMMAND xcrun --show-sdk-path
        OUTPUT_VARIABLE CMAKE_OSX_SYSROOT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

find_package(GTest REQUIRED)

add_executable(test_state_vector tests/cpp/test_state_vector.cpp)
target_link_libraries(test_state_vector qnoise_core GTest::GTest GTest::Main)

add_executable(test_density_matrix tests/cpp/test_density_matrix.cpp)
target_link_libraries(test_density_matrix qnoise_core GTest::GTest GTest::Main)

set_target_properties(qnoise_cpp PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/python/qnoise
)