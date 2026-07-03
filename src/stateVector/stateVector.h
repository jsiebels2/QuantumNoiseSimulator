#ifndef STATE_VECTOR
#define STATE_VECTOR

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

class stateVector {
    public:
        explicit stateVector(int numQubits);

        //apply the corresponding gate that gets past through
        void applyGate(const string gateName, const vector<int>& qubitIndices);
        void applyGate(const MatrixXcd& gateMatrix, const vector<int>& qubitIndices);

        int numQubits() const { return _n_qubits; }
        int dimensions() const { return dimensions_; }
        VectorXcd getCurrentState() const { return data_; }

    private:
        MatrixXcd constructFullMatrix(const MatrixXcd& gateMatrix, const vector<int>& qubitIndices);

        int _n_qubits;
        int dimensions_; // 2^n
        VectorXcd data_;
};

#endif