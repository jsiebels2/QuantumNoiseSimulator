#include <Eigen/Dense>
#include <cmath>
#include "phase_damping.hpp"
#include "../../constants.hpp"


PhaseDamping::PhaseDamping(double g) : _gamma(g) {}

vector<Eigen::MatrixXcd> PhaseDamping::getKrausOps() const {
    Matrix2cd K0, K1;

    K0 << 1,      0,
          0, sqrt(1 - _gamma);

    K1 << 0,    0,
          0, sqrt(_gamma);

    return { K0, K1 };
}