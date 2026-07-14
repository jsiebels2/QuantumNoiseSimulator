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

    if(qubitIndices.size() > 1 && it->second.rows() == 2 && it->second.cols() == 2) {
        //If the same single qubit gate needs to be applied to multiple qubits
        for(auto& qbit: qubitIndices) {
            applyGate(it->second, {qbit});
        }
    }
    else {
        //Two qubit state
        applyGate(it->second, qubitIndices);
    }
}

void stateVector::applyGate(const MatrixXcd& gateMatrix, const vector<int>& qubitIndices) {
    int k = qubitIndices.size();
    int groupSize = 1 << k;
    
    assert(gateMatrix.rows() == groupSize && gateMatrix.cols() == groupSize);
 
    for(int i = 0; i < dimensions_; i++) {
        // isBase = false when we have already used this state's target qubit
        bool isBase = true;
        for(auto& qubit: qubitIndices) {
            if((i >> qubit) & 1) {
               isBase = false;
               break; 
            }
        }
        if(!isBase) { continue; }

        VectorXcd amplitudes(groupSize);
        vector<int> indices(groupSize);
        for(int j = 0; j < groupSize; j++) {
            int idx = i;
            for(int n = 0; n < k; n++) {
                if((j >> n) & 1 ) idx |= (1 << qubitIndices[k - 1 - n]);
            }
            indices[j] = idx;
            amplitudes[j] = data_[idx];
        }

        for(int r = 0; r < groupSize; r++) {
            complex<double> sum = 0;
            for(int c = 0; c < groupSize; c++) {
                sum += gateMatrix(r, c) * amplitudes[c];
            }
            data_[indices[r]] = sum;
        }
    }
}