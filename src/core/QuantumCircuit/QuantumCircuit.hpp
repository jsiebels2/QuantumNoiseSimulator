#ifndef QUANTUMCIRCUIT 
#define QUANTUMCIRCUIT 

#include <map>
#include <optional>
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
        stateVector executeWithoutNoise(std::optional<int> numShots = std::nullopt);
        DensityMatrix executeWithPosteriorNoise(string noiseChannel, double gamma, std::optional<int> numShots = std::nullopt);
        DensityMatrix executeConcurrentNoise(string noiseChannel, double gamma, std::optional<int> numShots = std::nullopt);

        vector<GateOp> getCircuit() { return circuit; }
        VectorXcd getCurrentStateVector() { return currentSv.getCurrentState(); }
        MatrixXcd getCurrentDensityMatrix() { return currentDm.getCurrentState(); }

    private:
        int _n_qubits;
        vector<GateOp> circuit;
        stateVector currentSv;
        DensityMatrix currentDm;
        std::mt19937 gen;
        double getProbZeroSv(int qubit);
        double getProbZeroDm(int qubit);
        void collapseStateVector(int qubit, int m);
        void collapseDensityMatrix(int qubit, int m);
        void measureStateVector(vector<int> qubit);
        void measureDensityMatrix(vector<int> qubits);
        MatrixXcd getMeasurementOperator(int qubit, int m);

};

#endif