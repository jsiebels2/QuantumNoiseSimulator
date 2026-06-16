// src/bindings/pybind_module.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <pybind11/eigen.h>
#include <pybind11/numpy.h>
#include "stateVector/stateVector.h"

namespace py = pybind11;

PYBIND11_MODULE(qnoise_cpp, m) {
    m.doc() = "Quantum noise simulator C++ core";

    py::class_<stateVector>(m, "StateVector")
        // Methods
        .def(py::init<int>(), py::arg("num_qubits"),
            "Create am |0...0> state with the given number of qubits")

        .def("applySingleQubitGate", &stateVector::applySingleQubitGate, py::arg("gate"), py::arg("qubit"),
            "Apply a single qubit gate to the specified qubit")

        .def("applyTwoQubitGate", &stateVector::applyTwoQubitGate, py::arg("gate"), py::arg("target"), py::arg("control"),
            "Apply a two qubit gate with the specified control and target qubits")

        .def_property_readonly("getCurrentState", &stateVector::getCurrentState,
            "Get the current state of the state vector")

        .def_property_readonly("getDimensions", &stateVector::dimensions,
            "Get the dimensions of the state vector")

        .def_property_readonly("getQubits", &stateVector::numQubits,
            "Get the number of qubits of the state vector");
}