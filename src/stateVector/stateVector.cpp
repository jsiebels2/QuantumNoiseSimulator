#include "stateVector.h"
#include <cmath>
#include <Eigen/Dense>
#include <unsupported/Eigen/KroneckerProduct>
#include <iostream>

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

void stateVector::applySingleQubitGate(Matrix2cd& gate, int qubit) {
    MatrixXcd fullMatrix = constructFullMatrix(gate, qubit);
    data_ = fullMatrix * data_;
}

void stateVector::applyTwoQubitGate(Matrix4cd& gate, int controlBit, int targetBit) {
    if(controlBit == targetBit) {
        throw("Cannot apply a two qubit gate on the same qubit");
    }

    if(abs(controlBit - targetBit) != 1) {
        int minQubit = min(controlBit, targetBit);
        int maxQubit = max(controlBit, targetBit);

        int temp = minQubit;
        
        for(int i = minQubit; i < maxQubit - 1; i++) {
            applySwap(i, i + 1);
            temp++;
        }

        MatrixXcd fullMatrix = constructFullMatrix(gate, temp, maxQubit);
        data_ = fullMatrix * data_;

        for(int i = maxQubit - 1; i > minQubit; i--) {
            applySwap(i-1, i);
        }
    }
    else {
       MatrixXcd fullMatrix = constructFullMatrix(gate, controlBit, targetBit);
       data_ = fullMatrix * data_; 
    }
}

MatrixXcd stateVector::constructFullMatrix(Matrix2cd& gate, int target) {
    MatrixXcd result = (target == 0) ? gate : I;

    for(int i = 1; i < _n_qubits; i++) {
        if(i == target) {
            result = kroneckerProduct(result, gate).eval();
        }
        else {
            result = kroneckerProduct(result, I).eval();
        }
    }

    return result;
}

MatrixXcd stateVector::constructFullMatrix(Matrix4cd& gate, int controlBit, int targetBit) {
    if(_n_qubits == 2) {
        return gate;
    }
    
    int minQubit = min(controlBit, targetBit);
    MatrixXcd result;

    if(minQubit == 0) {
        result = gate;
    }
    else {
        result = I;
    }

    for(int i = 1; i < _n_qubits; i++) {
        if(i == minQubit) {
            result = kroneckerProduct(result, gate).eval();
            cout << result;
            i++;
        }
        else {
            result = kroneckerProduct(result, I).eval();
        }
    }
    cout << endl;

    return result;
}

void stateVector::applySwap(int qubit1, int qubit2) {
    Matrix4cd swap;

    swap << 1, 0, 0, 0,
            0, 0, 1, 0,
            0, 1, 0, 0,
            0, 0, 0, 1;

    applyTwoQubitGate(swap, qubit1, qubit2);
}