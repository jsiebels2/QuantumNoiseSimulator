#include "core/QuantumCircuit/QuantumCircuit.hpp"
#include "core/constants.hpp"
#include <cmath>
#include <random>

NoiseChannel createNoiseChannel(string nc, double gamma) {
    if(nc == "amplitude-damping") return NoiseChannel::AmplitudeDampingChannel(gamma);
    if(nc == "depolarizing-noise") return NoiseChannel::DepolarizingNoiseChannel(gamma);
    if(nc == "phase-damping") return NoiseChannel::PhaseDampingChannel(gamma);
    if(nc == "bit-phase-flips") return NoiseChannel::BitPhaseFlipChannel(gamma);
    
    throw invalid_argument("Noise Channel " + nc + " is not currently supported\n"
        "Supported noise channels:\n"
        "amplitude-damping\n"
        "depolarizing-noise\n"
        "phase-damping\n"
        "bit-phase-flips");
}

QuantumCircuit::QuantumCircuit(int qubits) : _n_qubits(qubits), currentSv(qubits), gen(std::random_device{}()) {}

void QuantumCircuit::addGate(string gate, vector<int> qubits) {
    GateOp operation(gate, qubits);
    circuit.push_back(operation);
}

stateVector QuantumCircuit::executeWithoutNoise() {
    stateVector sv(_n_qubits);

    for(auto& op: circuit) {
        if(op.gate == "measure") {
            measureStateVector(op.qubits);
            sv = currentSv;
            continue;
        }

        sv.applyGate(op.gate, op.qubits);
        currentSv = sv;
    }

    return sv;
}

DensityMatrix QuantumCircuit::executeWithPosteriorNoise(string noiseChannel, double gamma) {
    stateVector sv(_n_qubits);

    for(auto& op: circuit) {
        sv.applyGate(op.gate, op.qubits);
    }

    DensityMatrix densityMatrix = DensityMatrix::fromStateVector(sv);
    currentDm = densityMatrix;
    NoiseChannel nc = createNoiseChannel(noiseChannel, gamma);

    for(int i = 0; i < sv.dimensions(); i++) {
        densityMatrix.applyKrausOperator(nc.getKrausOps(), {i});
    }

    return densityMatrix;
}

DensityMatrix QuantumCircuit::executeConcurrentNoise(string noiseChannel, double gamma) {
    stateVector sv(_n_qubits);
    DensityMatrix dm = DensityMatrix::fromStateVector(sv);
    currentDm = dm;
    NoiseChannel nc = createNoiseChannel(noiseChannel, gamma);

    for(auto& op: circuit) {
        dm.applyGate(op.gate, op.qubits);
        for(auto& q: op.qubits) {
            dm.applyKrausOperator(nc.getKrausOps(), {q});
        }
        currentDm = dm; //this may need to change
    }

    return dm;
}

void QuantumCircuit::measureStateVector(vector<int> qubits) {
    for(auto& q: qubits) {
        double probZero = getProbZero(q);
        int state = 0;

        std::uniform_real_distribution<double> distrib(0.0, 1.0);
        double radndomNum = distrib(gen);
        if(radndomNum > probZero) state = 1;

        collapseStateVector(q, state);
    }
}

double QuantumCircuit::getProbZero(int qubit) {
    double probZero = 0;
    int svSize = currentSv.dimensions();
    Eigen::VectorXcd sv = currentSv.getCurrentState();

    //gets our probabilities
    for(int i = 0; i < svSize; i++) {
        if(((i >> qubit) & 1) == 0) {
            probZero += std::pow(std::abs(sv[i]), 2);
        }
    }

    return probZero;
}

void QuantumCircuit::collapseStateVector(int qubit, int m) {
    VectorXcd sv = currentSv.getCurrentState();

    for(int i = 0; i < currentSv.dimensions(); i++) {
        if((i >> qubit) != m) {
            sv[i] = 0.0;
        }
    }

    sv = sv / sv.norm();

    currentSv.changeStateVector(sv);
}
