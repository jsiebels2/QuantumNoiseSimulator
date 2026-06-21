#include "density_matrix.hpp"
#include <iostream>
#include <Eigen/Dense>
#include <unsupported/Eigen/KroneckerProduct>

using namespace std;
using namespace Eigen;

DensityMatrix DensityMatrix::fromStateVector(const stateVector& sv) {
    DensityMatrix dm;
    dm._n_qubits = sv.numQubits();
    dm._dim = 1 << dm._n_qubits; // 2^n
    dm._data = sv.getCurrentState() * sv.getCurrentState().adjoint();

    return dm;
}

DensityMatrix DensityMatrix::maximallyMixed(int numQubits) {
    DensityMatrix dm;
    dm._n_qubits = numQubits;
    dm._dim = 1 << numQubits; // 2^n
    dm._data = MatrixXcd::Identity(dm._dim, dm._dim) / static_cast<double>(dm._dim);

    return dm;
}

DensityMatrix DensityMatrix::fromMixedState(int numQubits, int basisStateIndex) {
    DensityMatrix dm;
    dm._n_qubits = numQubits;
    dm._dim = 1 << numQubits; // 2^n
    dm._data = MatrixXcd::Zero(dm._dim, dm._dim);
    dm._data(basisStateIndex, basisStateIndex) = 1.0;

    return dm;
}

void DensityMatrix::applyGate(const MatrixXcd& gate, const vector<int>& qubitIndices) {
    MatrixXcd fullGate = constructFullGate(gate, qubitIndices);
    MatrixXcd uDagger = fullGate.adjoint();
    _data = fullGate * _data * uDagger;
}

void DensityMatrix::applyKrausOperator(const vector<MatrixXcd>& krausOp, const vector<int>& qubitIndices) {
    MatrixXcd rhoPrime = MatrixXcd::Zero(_dim, _dim);

    for(const auto& K: krausOp) {
        MatrixXcd fullK = constructFullGate(K, qubitIndices);
        rhoPrime += fullK * _data * fullK.adjoint();
    }
    _data = rhoPrime;
}

double DensityMatrix::trace() const {
    return _data.trace().real();
}

double DensityMatrix::purity() const {
    return (_data * _data).trace().real();
}

MatrixXcd DensityMatrix::constructFullGate(const MatrixXcd& gate, const vector<int>& qubitIndices) {
    MatrixXcd fullGate(1,1);
    fullGate(0,0) = 1.0; // Start with identity

    for(int i = 0; i < _n_qubits; i++) {
        if(find(qubitIndices.begin(), qubitIndices.end(), i) != qubitIndices.end()) {
            fullGate = kroneckerProduct(fullGate, gate).eval();
        } else {
            fullGate = kroneckerProduct(fullGate, MatrixXcd::Identity(2, 2)).eval();
        }
    } 

    return fullGate;
}
