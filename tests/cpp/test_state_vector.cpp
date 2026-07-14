#include <gtest/gtest.h>
#include <Eigen/Dense>
#include "core/stateVector/stateVector.h"

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
    expected2 << complex<double>(0.0, 0.0), complex<double>(1.0, 0.0), complex<double>(0.0, 0.0), complex<double>(0.0, 0.0);

    sv.applyGate("x", {0});

    EXPECT_TRUE(sv.getCurrentState().isApprox(expected2));

    VectorXcd expected3(4);
    expected3 << complex<double>(0.0, 0.0), complex<double>(0.0, 0.0), complex<double>(0.0, 0.0), complex<double>(1.0, 0.0);

    sv.applyGate("x", {1});

    EXPECT_TRUE(sv.getCurrentState().isApprox(expected3)) << "The expected state vector was: " << expected3 << "\nBut the actual state vectore was: " << sv.getCurrentState() << endl;
}

TEST(StateVectorTest, TwoQubitGateOnTwoQubitSystem) {
    stateVector sv(2);

    sv.applyGate("h", {0, 1});

    VectorXcd ex1(4);
    ex1 << complex<double>(0.5, 0), complex<double>(0.5, 0), complex<double>(0.5, 0), complex<double>(0.5, 0);

    EXPECT_TRUE((sv.getCurrentState().isApprox(ex1)));

    sv.applyGate("cx", {0, 1});

    VectorXcd ex2(4);
    ex2 << complex<double>(0.5, 0.0), complex<double>(0.5, 0.0), complex<double>(0.5, 0.0), complex<double>(0.5, 0.0);

    EXPECT_TRUE((sv.getCurrentState().isApprox(ex2)));
}