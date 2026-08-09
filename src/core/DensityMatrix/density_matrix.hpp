#ifndef DENSITY_MATRIX
#define DENSITY_MATRIX

#include <Eigen/Dense>
#include <iostream>
#include "../stateVector/stateVector.h"

using namespace std;
using namespace Eigen;


class DensityMatrix {
    public:
        // Constructors
        static DensityMatrix fromStateVector(const stateVector& stateVec); 

        // Operations
        void applyGate(const string gateName, const vector<int>& qubitIndices);
        void applyKrausOperator(const vector<MatrixXcd>& krausOp, const vector<int>& qubitIndices);

        // Getters
        int numQubits() const { return _n_qubits; }
        int dimensions() const { return _dim; }
        MatrixXcd getCurrentState() const { return _data; }

        double trace() const;
        double purity() const;
    private:
        int _n_qubits;
        int _dim; // 2^n
        MatrixXcd _data;
        MatrixXcd applyGate(const MatrixXcd& gate, const vector<int>& qubitIndices);
        MatrixXcd tensoredNoiseChannels(const Matrix2cd& krausOp);
};

#endif