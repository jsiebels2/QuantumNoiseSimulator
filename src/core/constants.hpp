#include <Eigen/Dense>
#include <cmath>

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
        m << 0, (0.0, -1.0),
             (0.0, 1.0), 0;
        return m;
    };
}
