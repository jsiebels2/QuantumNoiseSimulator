#include "stateVector.h"
#include <cmath>
#include <Eigen/Dense>
#include <unsupported/Eigen/KroneckerProduct>
#include <iostream>
#include <core/constants.hpp>

using namespace std;
using namespace Eigen;

/*
This is currently not optimized to be able able to scale because of the construction of the full matrix.
Every full matrix that we are multiplying by the state vector when applying our gates is 2^n x 2^n dimensions,
which multiplying by another statevector of 2^nx2^n gives a time of 4^n which is not practical. This should be scaled
before completeion.
*/

Matrix2cd I = MatrixXcd::Identity(2,2);

stateVector::stateVector(int numQubits) {
    _n_qubits = numQubits;
    dimensions_ = pow(2, numQubits);
    
    data_ = VectorXcd::Zero(dimensions_);
    data_[0] = {1.0, 0.0};
} 

void stateVector::applyGate(const string gateName, const vector<int>& qubitIndices) {
    auto it = Qnoise::gateMap.find(gateName);

    if(it == Qnoise::gateMap.end()) {
        throw invalid_argument("Gate not supported: " + gateName);
    }

    applyGate(it->second, qubitIndices);
}

void stateVector::applyGate(const MatrixXcd& gateMatrix, const vector<int>& qubitIndices) {
    MatrixXcd fullgate = constructFullMatrix(gateMatrix, qubitIndices);
    data_ = fullgate * data_;
}

MatrixXcd stateVector::constructFullMatrix(const MatrixXcd& gateMatrix, const vector<int>& qubitIndices) {
    MatrixXcd fullGate(1,1);
    fullGate(0,0) = 1.0; // Start with identity

    for(int i = 0; i < _n_qubits; i++) {
        if(find(qubitIndices.begin(), qubitIndices.end(), i) != qubitIndices.end()) {
            fullGate = kroneckerProduct(fullGate, gateMatrix).eval();
        } else {
            fullGate = kroneckerProduct(fullGate, MatrixXcd::Identity(2, 2)).eval();
        }
    } 

    return fullGate;
}