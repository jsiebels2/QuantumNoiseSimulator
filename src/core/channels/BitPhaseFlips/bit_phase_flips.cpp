#include "bit_phase_flips.hpp"
#include "../../constants.hpp"
#include <cmath>
#include <Eigen/Dense>

BitPhaseFlips::BitPhaseFlips(double p) {
    _p = p;
}

vector<MatrixXcd> BitPhaseFlips::getKrausOps() const {
    Matrix2cd K0, K1;

    K0 = sqrt(_p) * MatrixXcd::Identity(2,2);

    K1 = sqrt(1 - _p) * Qnoise::PAULI_Y();

    return { K0, K1 };
}