#include <Eigen/Dense>
#include <cmath>
#include "depolarizing_noise.hpp"
#include "../../constants.hpp"


DepolarizingNoise::DepolarizingNoise(double p) : _p(p) {}

vector<Eigen::MatrixXcd> DepolarizingNoise::getKrausOps() const {
    Matrix2cd K0, K1, K2, K3;

    K0 = sqrt(1 - (3 * _p)/4) * Matrix2cd::Identity(2,2);
    K1 = sqrt(_p/4) * Qnoise::PAULI_X();
    K2 = sqrt(_p/4) * Qnoise::PAULI_Y();
    K3 = sqrt(_p/4) * Qnoise::PAULI_Z();

    return { K0, K1, K2, K3 };
}