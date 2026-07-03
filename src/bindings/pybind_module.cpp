// src/bindings/pybind_module.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <pybind11/eigen.h>
#include <pybind11/numpy.h>
#include "stateVector/stateVector.h"
#include "core/DensityMatrix/density_matrix.hpp"
#include "core/channels/AmplitudeDamping/amplitude_damping.hpp"
#include "core/channels/BitPhaseFlips/bit_phase_flips.hpp"
#include "core/channels/DepolarizingNoise/depolarizing_noise.hpp"
#include "core/channels/PhaseDamping/phase_damping.hpp"

namespace py = pybind11;

PYBIND11_MODULE(qnoise_cpp, m) {
    m.doc() = "Quantum noise simulator C++ core";

    py::class_<stateVector>(m, "StateVector")
        // Methods
        .def(py::init<int>(), py::arg("num_qubits"),
            "Create am |0...0> state with the given number of qubits")

        .def("applyGate", py::overload_cast<const string, const vector<int>&>(&stateVector::applyGate), py::arg("gateName"), py::arg("qubitIndices"),
            "Apply a single qubit gate to the specified qubit")

        .def("applyGate", py::overload_cast<const MatrixXcd&, const vector<int>&>(&stateVector::applyGate), py::arg("gateMatrix"), py::arg("qubitIndices"),
            "Apply a single qubit gate to the specified qubit")

        .def_property_readonly("getCurrentState", &stateVector::getCurrentState,
            "Get the current state of the state vector")

        .def_property_readonly("getDimensions", &stateVector::dimensions,
            "Get the dimensions of the state vector")

        .def_property_readonly("getQubits", &stateVector::numQubits,
            "Get the number of qubits of the state vector");
    
    py::class_<DensityMatrix>(m, "DensityMatrix")
        //Methods
        .def_static("fromStateVector", &DensityMatrix::fromStateVector, py::arg("state_vector"),
            "Create a density matrix from a state vector")
        
        .def("applyGate", py::overload_cast<const string, const vector<int>&>(&DensityMatrix::applyGate), py::arg("gateName"), py::arg("qubit_indices"),
            "Apply a gate to the density matrix")   
        
        .def("applyGate", py::overload_cast<const MatrixXcd&, const vector<int>&>(&DensityMatrix::applyGate), py::arg("gateMatrix"), py::arg("qubit_indices"),
            "Apply a gate to the density matrix")
        
        .def("applyKrausOperator", &DensityMatrix::applyKrausOperator, py::arg("kraus_operators"), py::arg("qubit_indices"),
            "Apply kraus operators to the density matrix")

        .def("numQubits", &DensityMatrix::numQubits,
            "Get the number of qubits")

        .def("dimensions", &DensityMatrix::dimensions,
            "Get the dimensions of the density matrix")

        .def("getDensityMatrix", &DensityMatrix::getDensityMatrix,
            "Get the denstiy matrix")

        .def("trace", &DensityMatrix::trace,
            "Get the trace of the density matrix")

        .def("purity", &DensityMatrix::purity,
            "Get the purity of the density matrix");
        
    py::class_<KrausChannel>(m, "KrausChannel")
        //methods
        .def("getKrausOps", &KrausChannel::getKrausOps)
        .def("apply", &KrausChannel::apply);
    
    py::class_<AmplitudeDamping, KrausChannel>(m, "AmplitudeDamping")
        //methods
        .def(py::init<double>(), py::arg("gamma"));
    
    py::class_<BitPhaseFlips, KrausChannel>(m, "BitPhaseFlip")
        //methods
        .def(py::init<double>(), py::arg("p-value"));

    py::class_<DepolarizingNoise, KrausChannel>(m, "DepolarizingNoise")
        //methods
        .def(py::init<double>(), py::arg("p-value"));

    py::class_<PhaseDamping, KrausChannel>(m, "PhaseDamping")
        //methods
        .def(py::init<double>(), py::arg("gamma"));
}