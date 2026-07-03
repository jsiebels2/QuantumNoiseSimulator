#include <Eigen/Dense>
#include <cmath>
#include "amplitude_damping.hpp"
#include "../../constants.hpp"

using namespace std;

AmplitudeDamping::AmplitudeDamping(double g) : _gamma(g) {}

vector<Eigen::MatrixXcd> AmplitudeDamping::getKrausOps() const {
    Matrix2cd K0, K1;

    K0 << 1,    0,
          0, sqrt(1 - _gamma);

    K1 << 0, sqrt(_gamma),
          0,    0;

    return { K0, K1 };
}