#ifndef QUANTUMCIRCUIT 
#define QUANTUMCIRCUIT 

#include <map>
#include "core/stateVector/stateVector.h"
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

        vector<GateOp> getCircuit() { return circuit; }
    private:
        int _n_qubits;
        vector<GateOp> circuit;
};

#endif