#include "density_matrix.hpp"
#include <iostream>
#include <Eigen/Dense>
#include <unsupported/Eigen/KroneckerProduct>
#include <unordered_map>
#include "core/constants.hpp"

using namespace std;
using namespace Eigen;

DensityMatrix DensityMatrix::fromStateVector(const stateVector& sv) {
    DensityMatrix dm;
    dm._n_qubits = sv.numQubits();
    dm._dim = 1 << dm._n_qubits; // 2^n
    dm._data = sv.getCurrentState() * sv.getCurrentState().adjoint();

    return dm;
}

void DensityMatrix::applyGate(const string gateName, const vector<int>& qubitIndices) {
    auto it = Qnoise::gateMap.find(gateName);

    if(it == Qnoise::gateMap.end()) {
        throw invalid_argument("Gate not supported: " + gateName);
    }

    if(qubitIndices.size() > 1 && it->second.rows() == 2 && it->second.cols() == 2) {
        //If the same single qubit gate needs to be applied to multiple qubits
        for(auto& qbit: qubitIndices) {
            _data = applyGate(it->second, {qbit});
        }
    }
    else {
        //Two qubit state
        _data = applyGate(it->second, qubitIndices);
    }
}

MatrixXcd DensityMatrix::applyGate(const MatrixXcd& gate, const vector<int>& qubitIndices) {
    int k = qubitIndices.size();
    int groupSize = 1 << k;
    MatrixXcd gateAdjoint = gate.adjoint();
    MatrixXcd newDm = _data;

    for(int row = 0; row < _dim; row++) {
        for(int col = 0; col < _dim; col++) {
            bool isBase = true;
            for(const auto& qubit: qubitIndices) {
                if(((row >> qubit) & 1) || ((col >> qubit) & 1)) {
                    isBase = false;
                    break;
                }
            }
            if(!isBase) continue;

            MatrixXcd amplitudes(groupSize, groupSize);
            MatrixXi rowIndices(groupSize, groupSize);
            MatrixXi colIndices(groupSize, groupSize);
            for(int r = 0; r < groupSize; r++) {
                for(int c = 0; c < groupSize; c++) {
                    int idxR = row;
                    int idxC = col;
                    for(int i = 0; i < k; i++) {
                        if(((r >> i) & 1)) {
                            idxR |= 1 << qubitIndices[k - 1 - i];
                        }
                        if((c >> i) & 1) {
                            idxC |= 1 << qubitIndices[k - 1 - i];
                        }
                    }
                    amplitudes(r, c) = newDm(idxR, idxC);
                    rowIndices(r, c) = idxR;
                    colIndices(r, c) = idxC;
                }
            }

            MatrixXcd newBlock = gate * amplitudes * gateAdjoint;

            for(int i = 0; i < groupSize; i++) {
                for(int j = 0; j < groupSize; j++) {
                    newDm(rowIndices(i,j), colIndices(i,j)) = newBlock(i,j);
                }
            }
        }
    }

    return newDm;
}

void DensityMatrix::applyKrausOperator(const vector<MatrixXcd>& krausOp, const vector<int>& qubitIndices) {
    MatrixXcd rhoPrime = MatrixXcd::Zero(_dim, _dim);

    for(const auto& K: krausOp) {
        MatrixXcd newK = K;
        if(qubitIndices.size() > 1) {
            newK = tensoredNoiseChannels(K);
        }
        rhoPrime += applyGate(newK, qubitIndices);
    }
    _data = rhoPrime;
}

MatrixXcd DensityMatrix::tensoredNoiseChannels(const Matrix2cd& krausOp) {
    return kroneckerProduct(krausOp, krausOp);
}

double DensityMatrix::trace() const {
    return _data.trace().real();
}

double DensityMatrix::purity() const {
    return (_data * _data).trace().real();
}

