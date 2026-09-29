#ifndef QUANTUMCIRCUIT 
#define QUANTUMCIRCUIT 

#include <map>
#include <random>
#include "core/stateVector/stateVector.hpp"
#include "core/DensityMatrix/density_matrix.hpp"
#include "core/channels/noise_channels.hpp"

struct GateOp {
    std::string gate;
    std::vector<int> qubits;    

    GateOp(std::string gate, vector<int> qubits) : gate(gate), qubits(qubits) {};
};

class QuantumCircuit {
    public:
        explicit QuantumCircuit(int qubits);
        void addGate(string gate, vector<int> qubits);
        stateVector executeWithoutNoise();
        DensityMatrix executeWithPosteriorNoise(string noiseChannel, double gamma);
        DensityMatrix executeConcurrentNoise(string noiseChannel, double gamma);
        void measureStateVector(vector<int> qubit);

        vector<GateOp> getCircuit() { return circuit; }
        VectorXcd getCurrentStateVector() { return currentSv.getCurrentState(); }
        MatrixXcd getCurrentDensityMatrix() { return currentDm.getCurrentState(); }

    private:
        int _n_qubits;
        vector<GateOp> circuit;
        stateVector currentSv;
        DensityMatrix currentDm;
        std::mt19937 gen;
        double getProbZero(int qubit);
        void collapseStateVector(int qubit, int m);

};

#endif