#include "core/QuantumCircuit/QuantumCircuit.hpp"
#include "Eigen/src/Core/Matrix.h"
#include "core/DensityMatrix/density_matrix.hpp"
#include "core/constants.hpp"
#include "core/stateVector/stateVector.hpp"
#include "unsupported/Eigen/src/KroneckerProduct/KroneckerTensorProduct.h"
#include <cmath>
#include <cstddef>
#include <exception>
#include <random>
#include <string>

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

QuantumCircuit::QuantumCircuit(int qubits) : _n_qubits(qubits), currentSv(qubits), gen(std::random_device{}()), result(stateVector(qubits), std::pow(2, _n_qubits)) {}

void QuantumCircuit::addGate(string gate, vector<int> qubits) {
    GateOp operation(gate, qubits);
    circuit.push_back(operation);
}

//TODO
unordered_map<string, int> QuantumCircuit::executeCircuit(std::string noiseChannel, std::string noiseType, double gamma, std::optional<int> numShots) {
    int shots = numShots.value_or(1024);

    for(int i = 0; i < shots; i++) {
        currentSv = stateVector(_n_qubits);
        currentDm = DensityMatrix::fromStateVector(currentSv);

        if(noiseType == "concurrent-noise") {
            result.dm = executeConcurrentNoise(noiseChannel, gamma);
        }
        else if (noiseType == "posterior-noise"){
            result.dm = executeWithPosteriorNoise(noiseChannel, gamma);
        }
        else {
            result.sv = executeWithoutNoise();
        }
    }

    return result.measurmentStatistics;
}

stateVector QuantumCircuit::executeWithoutNoise() {    
    string bitString = string((size_t)_n_qubits, '-');

    for(auto& op: circuit) {
        if(op.gate == "measure") {
            for(auto& q: op.qubits) {
                int measurement = measureStateVector(q);
                bitString[q] = measurement + '0';
            }
            continue;
        }

        currentSv.applyGate(op.gate, op.qubits);
    }

    result.measurmentStatistics[bitString]++;
    return currentSv;
}

DensityMatrix QuantumCircuit::executeWithPosteriorNoise(string noiseChannel, double gamma) {
    string bitString = string((size_t)_n_qubits, '-');

    for(auto& op: circuit) {
        if(op.gate == "measure") {
            for(auto& q: op.qubits) {
                int measurement = measureStateVector(q);
                bitString[q] = measurement + '0';
            }
            continue;
        }
        currentSv.applyGate(op.gate, op.qubits);
    }

    DensityMatrix densityMatrix = DensityMatrix::fromStateVector(currentSv);
    currentDm = densityMatrix;
    NoiseChannel nc = createNoiseChannel(noiseChannel, gamma);

    for(int i = 0; i < _n_qubits; i++) {
        densityMatrix.applyKrausOperator(nc.getKrausOps(), {i});
        currentDm = densityMatrix;
    }

    result.measurmentStatistics[bitString]++;
    return densityMatrix;
}

DensityMatrix QuantumCircuit::executeConcurrentNoise(string noiseChannel, double gamma) {
    currentDm = DensityMatrix::fromStateVector(currentSv);
    NoiseChannel nc = createNoiseChannel(noiseChannel, gamma);
    string bitString = std::string((size_t)_n_qubits, '-');

    for(auto& op: circuit) {
        if(op.gate == "measure") {
            for(auto& q: op.qubits) {
                int measurement = measureDensityMatrix(q);
                bitString[q] = measurement + '0';
            }
            continue;
        }
        currentDm.applyGate(op.gate, op.qubits);
        for(auto& q: op.qubits) {
            currentDm.applyKrausOperator(nc.getKrausOps(), {q});
        }
    }

    result.measurmentStatistics[bitString]++;
    return currentDm;
}

int QuantumCircuit::measureStateVector(int q) {
    double probZero = getProbZeroSv(q);
    int state = 0;

    std::uniform_real_distribution<double> distrib(0.0, 1.0);
    double radndomNum = distrib(gen);
    if(radndomNum > probZero) state = 1;

    collapseStateVector(q, state);
    return state;
}

int QuantumCircuit::measureDensityMatrix(int q) {
    double probZero = getProbZeroDm(q);
    int state = 0;

    std::uniform_real_distribution<double> distrib(0.0, 1.0);
    double radndomNum = distrib(gen);
    if(radndomNum > probZero) state = 1;

    collapseDensityMatrix(q, state);
    return state;
}

double QuantumCircuit::getProbZeroSv(int qubit) {
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

double QuantumCircuit::getProbZeroDm(int qubit) {
    double probZero = 0.0;
    Eigen::MatrixXcd dm = currentDm.getCurrentState();

    //Just uses the diagonals
    for(int i = 0; i < currentDm.dimensions(); i++) {
        if(((i >> qubit) & 1) == 0) {
            probZero += dm(i,i).real();
        }
    }

    return probZero;
}

void QuantumCircuit::collapseStateVector(int qubit, int m) {
    VectorXcd sv = currentSv.getCurrentState();

    for(int i = 0; i < currentSv.dimensions(); i++) {
        if(((i >> qubit) & 1) != m) {
            sv[i] = 0.0;
        }
    }

    sv = sv / sv.norm();

    currentSv.changeStateVector(sv);
}

void QuantumCircuit::collapseDensityMatrix(int qubit, int m) {
    MatrixXcd dm = currentDm.getCurrentState();
    MatrixXcd measurementOp = getMeasurementOperator(qubit, m);
    MatrixXcd p_ip = measurementOp * dm;

    currentDm.updateDensityMatrix(((p_ip * measurementOp) / p_ip.trace()));
}

MatrixXcd QuantumCircuit::getMeasurementOperator(int qubit, int m) {
    MatrixXcd outerProd;
    MatrixXcd id = Eigen::MatrixXcd::Identity(2,2);
    MatrixXcd finalProd = Eigen::MatrixXcd::Identity(1,1);

    if(m == 0) {
        outerProd = Eigen::Vector2cd::Unit(2, 0) * Eigen::Vector2cd::Unit(2, 0).adjoint();
    } else {
        outerProd = Eigen::Vector2cd::Unit(2, 1) * Eigen::Vector2cd::Unit(2, 1).adjoint();
    }

    for(int i = currentDm.numQubits() - 1; i >= 0; i--) {
        if(i == qubit) { //accounts for little endian
            finalProd = Eigen::kroneckerProduct(finalProd,outerProd).eval();
            continue;
        }
        finalProd = Eigen::kroneckerProduct(finalProd, id).eval();
    }

    return finalProd;
}
