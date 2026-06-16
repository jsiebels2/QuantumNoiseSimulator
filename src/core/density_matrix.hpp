#ifndef DENSITY_MATRIX
#define DENSITY_MATRIX

#include <Eigen/Dense>
#include <iostream>
#include "stateVector/stateVector.h"

using namespace std;
using namespace Eigen;


class DensityMatrix {
    public:
        // Constructors
        static DensityMatrix fromStateVector(const stateVector& stateVec); 
        static DensityMatrix maximallyMixed(int numQubits);
        static DensityMatrix fromMixedState(int numQubits, int basisStateIndex);

        // Operations
        void applyGate(const MatrixXcd& gate, const vector<int>& qubitIndices);
        void applyKrausOperator(const vector<MatrixXcd>& krausOp, const vector<int>& qubitIndices);

        // Getters
        int numQubits() const { return _n_qubits; }
        int dimensions() const { return _dim; }
        MatrixXcd getDensityMatrix() const { return _data; }

        double trace() const;
        double purity() const;
        complex<double> expectationValue(const MatrixXcd& observable) const;
    private:
        int _n_qubits;
        int _dim; // 2^n
        MatrixXcd _data;
        MatrixXcd constructFullGate(const MatrixXcd& gate, const vector<int>& qubitIndices);
};

#endif