#include <Eigen/Dense>
#include <cmath>
#include "amplitude_dampening.hpp"
#include "../constants.hpp"

using namespace std;

AmplitudeDampening::AmplitudeDampening(double g) : _gamma(g) {}

vector<Eigen::MatrixXcd> AmplitudeDampening::getKrausOps() const {
    Matrix2cd K0, K1;

    K0 << 1,    0,
          0, sqrt(1 - _gamma);

    K1 << 0, sqrt(_gamma),
          0,    0;

    return { K0, K1 };
}