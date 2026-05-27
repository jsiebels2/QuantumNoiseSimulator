// src/bindings/pybind_module.cpp
#include <pybind11/pybind11.h>
namespace py = pybind11;

PYBIND11_MODULE(qnoise_cpp, m) {
    m.doc() = "Quantum noise simulator C++ core";
}