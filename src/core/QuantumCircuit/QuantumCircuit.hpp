#ifndef QUANTUMCIRCUIT 
#define QUANTUMCIRCUIT 

#include <optional>
#include <random>
#include <unordered_map>
#include "core/stateVector/stateVector.hpp"
#include "core/DensityMatrix/density_matrix.hpp"

struct GateOp {
    std::string gate;
    std::vector<int> qubits;    

    GateOp(std::string gate, vector<int> qubits) : gate(gate), qubits(qubits) {};
};

struct ExecutionResult {
    unordered_map<string, int> measurmentStatistics;
    stateVector sv;
    DensityMatrix dm;

    ExecutionResult(stateVector sv, size_t n) : sv(sv) {};
};

class QuantumCircuit {
    public:
        explicit QuantumCircuit(int qubits);
        void addGate(string gate, vector<int> qubits);
        unordered_map<string, int> executeCircuit(std::string noiseChannel, std::string noiseType, double gamma, std::optional<int> numShots);
        stateVector executeWithoutNoise();
        DensityMatrix executeWithPosteriorNoise(string noiseChannel, double gamma);
        DensityMatrix executeConcurrentNoise(string noiseChannel, double gamma);

        vector<GateOp> getCircuit() { return circuit; }
        VectorXcd getCurrentStateVector() { return currentSv.getCurrentState(); }
        MatrixXcd getCurrentDensityMatrix() { return currentDm.getCurrentState(); }

    private:
        int _n_qubits;
        vector<GateOp> circuit;
        ExecutionResult result;
        stateVector currentSv;
        DensityMatrix currentDm;
        std::mt19937 gen;
        double getProbZeroSv(int qubit);
        double getProbZeroDm(int qubit);
        void collapseStateVector(int qubit, int m);
        void collapseDensityMatrix(int qubit, int m);
        int measureStateVector(int qubit);
        int measureDensityMatrix(int qubits);
        MatrixXcd getMeasurementOperator(int qubit, int m);

};

#endif