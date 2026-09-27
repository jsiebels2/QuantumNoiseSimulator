#pragma once
#include <Eigen/Dense>
#include <cmath>
#include <unordered_map>
#include <numbers>
#include "channels/noise_channels.hpp"
#include <complex>
#include <unsupported/Eigen/KroneckerProduct>

namespace Qnoise {
    constexpr double PI = 3.1459265358979323846;

    inline Eigen::Matrix2cd PAULI_X() {
        Eigen::Matrix2cd m;
        m << 0, 1,
             1, 0;
        return m;
    };

    inline Eigen::Matrix2cd PAULI_Z() {
        Eigen::Matrix2cd m;
        m << 1, 0,
             0, -1;
        return m;
    };

    inline Eigen::Matrix2cd PAULI_Y() {
        Eigen::Matrix2cd m;
        m << 0, -1i,
            1i, 0;
        return m;
    };
    
    inline Eigen::Matrix2cd H() {
        Eigen::Matrix2cd m;
        m << 1/sqrt(2), 1/sqrt(2),
            1/sqrt(2), -1/sqrt(2);
        return m;
    };

    inline Eigen::Matrix4cd CNOT() {
        Eigen::Matrix4cd m;
        m << 1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 0, 1,
            0, 0, 1, 0;
        return m;
    }

    inline Eigen::Matrix4cd CZ() {
        Eigen::Matrix4cd m;
        m << 1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, -1;
        return m;
    }

    inline Eigen::Matrix4cd SWAP() {
        Eigen::Matrix4cd m;
        m << 1, 0, 0, 0,
            0, 0, 1, 0,
            0, 1, 0, 0,
            0, 0, 0, 1;
        return m;
    }

    inline Eigen::Matrix2cd S() {
        Eigen::Matrix2cd m;
        m << 1, 0,
            0, 1i;
        return m;
    }

    inline Eigen::Matrix2cd T() {
        Eigen::Matrix2cd m;
        m << 1, 0,
            0, std::exp((PI * 1i)/4.0);
        return m;
    }

    inline Eigen::Matrix2cd SX() {
        Eigen::Matrix2cd m;
        m << std::complex<double>(1.0, 1.0)/2.0, std::complex<double>(1.0, -1.0)/2.0,
            std::complex<double>(1.0, -1.0)/2.0, std::complex<double>(1.0, 1.0)/2.0;
        return m;
    }

     inline const unordered_map<string, MatrixXcd> gateMap = {
        {"x", PAULI_X()},
        {"y", PAULI_Y()},
        {"z", PAULI_Z()},
        {"h", H()},
        {"id", MatrixXcd::Identity(2,2)}, 
        {"s", S()},
        {"t", T()},
        {"sdg", S().adjoint()},
        {"td", T().adjoint()},
        {"sx", SX()},
        {"sw", SWAP()}
    };

    inline const unordered_map<string, MatrixXcd> controlGates = {
        {"cx", PAULI_X()},
        {"cz", PAULI_Z()}
    };

    inline Eigen::MatrixXcd constructControlMatrix(const string gateName, Eigen::Matrix2cd baseMatrix, const vector<int> controlQubits) {
        int n = controlQubits.size();
        cout << n << endl;
        MatrixXcd CNU, pN;
        Matrix2cd outerProdOne;

        outerProdOne << 0, 0,
                         0, 1;
        
        pN = outerProdOne;
        for(int i = 1; i < n; i++) {
            pN = Eigen::kroneckerProduct(pN, outerProdOne).eval();
        }

        CNU = Eigen::kroneckerProduct(MatrixXcd::Identity(std::pow(2, n), std::pow(2, n)) - pN, MatrixXcd::Identity(2,2)) + Eigen::kroneckerProduct(pN, baseMatrix);

        cout << "I have a valid CNU" << endl;

        return CNU;
    }

     inline Eigen::MatrixXcd getGateMatrix(const string gateName, const vector<int> controls={}) {
        if(controlGates.count(gateName) > 0) {
            auto cGate = controlGates.find(gateName);
            return constructControlMatrix(gateName, cGate->second, controls);
        }
        
        auto gate = gateMap.find(gateName);
        
        if(gate == gateMap.end()) {
            throw invalid_argument("Gate not supported: " + gateName);
        }
        
        return gate->second;
    }   
}
