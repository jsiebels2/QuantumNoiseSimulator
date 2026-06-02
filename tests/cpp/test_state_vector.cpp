#include <gtest/gtest.h>
#include <Eigen/Dense>
#include "stateVector/stateVector.h"

using namespace std;
using namespace Eigen;

TEST(StateVectorTest, CheckGroundState) {
    stateVector sv(1);

    VectorXcd expected(2);
    expected << complex<double>(1.0, 0.0), complex<double>(0.0, 0.0);

    EXPECT_TRUE(sv.getCurrentState().isApprox(expected));
}

TEST(StateVectorTest, CheckXGate) {
    stateVector sv(1);

    Matrix2cd X;
    X << 0, 1,
         1, 0;

    sv.applySingleQubitGate(X, 0);

    VectorXcd expected(2);
    expected << complex<double>(0.0, 0.0), complex<double>(1.0, 0.0);

    EXPECT_TRUE(sv.getCurrentState().isApprox(expected));
}