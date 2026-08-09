#include "core/QuantumCircuit/QuantumCircuit.hpp"

NoiseChannel createNoiseChannel(string nc, double gamma) {
    if(nc == "amplitude-damping") return NoiseChannel::AmplitudeDampingChannel(gamma);
    if(nc == "depolarizing-noise") return NoiseChannel::DepolarizingNoiseChannel(gamma);
    if(nc == "phase-damping") return NoiseChannel::PhaseDampingChannel(gamma);
    if(nc == "bit-phase-flips") return NoiseChannel::BitPhaseFlipChannel(gamma);
    
    throw invalid_argument("Noise Channel is not currently supported");
}

QuantumCircuit::QuantumCircuit(int qubits) : _n_qubits(qubits) {}

void QuantumCircuit::addGate(string gate, vector<int> qubits) {
    GateOp operation(gate, qubits);
    circuit.push_back(operation);
}

stateVector QuantumCircuit::executeWithoutNoise() {
    stateVector sv(_n_qubits);

    for(auto& op: circuit) {
        sv.applyGate(op.gate, op.qubits);
    }

    return sv;
}

DensityMatrix QuantumCircuit::executeWithPosteriorNoise(string noiseChannel, double gamma) {
    stateVector sv(_n_qubits);

    for(auto& op: circuit) {
        sv.applyGate(op.gate, op.qubits);
    }

    DensityMatrix densityMatrix = DensityMatrix::fromStateVector(sv);
    NoiseChannel nc = createNoiseChannel(noiseChannel, gamma);

    for(int i = 0; i < sv.dimensions(); i++) {
        densityMatrix.applyKrausOperator(nc.getKrausOps(), {i});
    }

    return densityMatrix;
}

DensityMatrix QuantumCircuit::executeConcurrentNoise(string noiseChannel, double gamma) {
    stateVector sv(_n_qubits);
    DensityMatrix dm = DensityMatrix::fromStateVector(sv);
    NoiseChannel nc = createNoiseChannel(noiseChannel, gamma);

    for(auto& op: circuit) {
        dm.applyGate(op.gate, op.qubits);
        for(auto& q: op.qubits) {
            dm.applyKrausOperator(nc.getKrausOps(), {q});
        }
    }

    return dm;
}
