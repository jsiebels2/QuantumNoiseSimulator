#ifndef STATE_VECTOR
#define STATE_VECTOR

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

class stateVector {
    public:
        explicit stateVector(int numQubits);

        //apply the corresponding gate that gets past through
        void applySingleQubitGate(Matrix2cd& gate, int qubit);
        void applyTwoQubitGate(Matrix4cd& gate, int controlBit, int targetBit);

        int numQubits() const { return _n_qubits; }
        int dimensione() const { return dimensions_; }
        VectorXcd getCurrentState() const { return data_; }

    private:
        MatrixXcd constructFullMatrix(Matrix2cd& gate, int targetQubit);
        MatrixXcd stateVector::constructFullMatrix(Matrix4cd& gate, int controlBit, int targetBit);
        void applySwap(int qubit1, int qubit2);

        int _n_qubits;
        int dimensions_; // 2^n
        VectorXcd data_;
};

#endif