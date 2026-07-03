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

    sv.applyGate("x", {0});

    VectorXcd expected(2);
    expected << complex<double>(0.0, 0.0), complex<double>(1.0, 0.0);

    EXPECT_TRUE(sv.getCurrentState().isApprox(expected));
}

TEST(StateVectorTest, TwoQubitSystem) {
    stateVector sv(2);

    int size = sv.getCurrentState().size();

    VectorXcd expected1(4);
    expected1 << complex<double>(1.0, 0.0), complex<double>(0.0, 0.0), complex<double>(0.0, 0.0), complex<double>(0.0, 0.0);

    EXPECT_EQ(size, 4);
    EXPECT_TRUE(sv.getCurrentState().isApprox(expected1));

    VectorXcd expected2(4);
    expected2 << complex<double>(0.0, 0.0), complex<double>(0.0, 0.0), complex<double>(1.0, 0.0), complex<double>(0.0, 0.0);

    sv.applyGate("x", {0});

    EXPECT_TRUE(sv.getCurrentState().isApprox(expected2));

    VectorXcd expected3(4);
    expected3 << complex<double>(0.0, 0.0), complex<double>(0.0, 0.0), complex<double>(0.0, 0.0), complex<double>(1.0, 0.0);

    sv.applyGate("x", {1});

    EXPECT_TRUE(sv.getCurrentState().isApprox(expected3));
}

// TEST(StateVectorTest, TwoQubitGateOnTwoQubitSystem) {
//     stateVector sv(2);

//     Matrix2cd H;
//     H << complex<double>(1/sqrt(2), 0), complex<double>(1/sqrt(2), 0),
//         complex<double>(1/sqrt(2),0 ), complex<double>(-1/sqrt(2), 0);

//     sv.applyGate("h", {0, 1});

//     VectorXcd ex1(4);
//     ex1 << complex<double>(0.5, 0), complex<double>(0.5, 0), complex<double>(0.5, 0), complex<double>(0.5, 0);

//     EXPECT_TRUE((sv.getCurrentState().isApprox(ex1)));

//     Matrix4cd CNOT;
//     CNOT << 1, 0, 0, 0,
//             0, 1, 0, 0,
//             0, 0, 0, 1,
//             0, 0, 1, 0;

//     sv.applyGate(CNOT, {0, 1});

//     VectorXcd ex2(4);
//     ex2 << complex<double>(0.5, 0.0), complex<double>(0.5, 0.0), complex<double>(0.5, 0.0), complex<double>(0.5, 0.0);

//     EXPECT_TRUE((sv.getCurrentState().isApprox(ex2)));
// }