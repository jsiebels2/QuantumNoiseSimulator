#pragma once
#include <Eigen/Dense>
#include <cmath>
#include <unordered_map>
#include "channels/noise_channels.hpp"

namespace Qnoise {
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

    inline const unordered_map<string, MatrixXcd> gateMap = {
        {"x", PAULI_X()},
        {"y", PAULI_Y()},
        {"z", PAULI_Z()},
        {"h", H()},
        {"id", MatrixXcd::Identity(2,2)},
        {"cx", CNOT()},
        {"cz", CZ()},
        {"sw", SWAP()}
    };
}
