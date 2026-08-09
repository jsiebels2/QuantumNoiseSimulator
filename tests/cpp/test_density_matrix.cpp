#include "core/DensityMatrix/density_matrix.hpp"
#include "core/stateVector/stateVector.h"
#include <gtest/gtest.h>

TEST(DensityMatrixTest, FromStateVector) {
    stateVector sv(1);
    DensityMatrix dm = DensityMatrix::fromStateVector(sv);

    MatrixXcd expected(2, 2);
    expected << complex<double>(1.0, 0.0), complex<double>(0.0, 0.0),
                complex<double>(0.0, 0.0), complex<double>(0.0, 0.0);

    EXPECT_TRUE(dm.getCurrentState().isApprox(expected));
}

TEST(DensityMatrixTest, FromStateVectorComplex) {
    stateVector sv(2);

    Matrix2cd H;
    H << 1/sqrt(2), 1/sqrt(2),
         1/sqrt(2), -1/sqrt(2);

    sv.applyGate("h", {0, 1});

    DensityMatrix dm = DensityMatrix::fromStateVector(sv);

    MatrixXcd expected(4, 4);
    expected << 0.25, 0.25, 0.25, 0.25,
                0.25, 0.25, 0.25, 0.25,
                0.25, 0.25, 0.25, 0.25,
                0.25, 0.25, 0.25, 0.25;

    EXPECT_TRUE(dm.getCurrentState().isApprox(expected));
}

TEST(DensityMatrixTest, GateApplicationAndMultiQubit) {
    stateVector sv(3);
    DensityMatrix dm = DensityMatrix::fromStateVector(sv);

    dm.applyGate("h", {0, 1, 2});

    MatrixXcd ex1 = MatrixXcd::Zero(8,8);
    for(int i = 0; i < ex1.rows(); i++) {
        for(int j = 0; j < ex1.cols(); j++) {
            ex1(i,j) = 0.125;
        }
    }

    EXPECT_TRUE(dm.getCurrentState().isApprox(ex1)) << "The density matrix was: " << dm.getCurrentState() << "\nExpected: " << ex1; 
}

TEST(DensityMatrixTest, TwoQubitGate) {
    stateVector sv(2);
    DensityMatrix dm = DensityMatrix::fromStateVector(sv);

    dm.applyGate("cx", {0, 1});

    MatrixXcd ex1 = MatrixXcd::Zero(4,4);
    ex1(0,0) = 1;
    
    EXPECT_TRUE(dm.getCurrentState().isApprox(ex1)) << "Expeceted:\n " << ex1 << "\nActual:\n " << dm.getCurrentState();

    dm.applyGate("x", {0, 1});
    dm.applyGate("cx", {0, 1});

    MatrixXcd ex2 = MatrixXcd::Zero(4,4);
    ex2(1,1) = 1;

    EXPECT_TRUE(dm.getCurrentState().isApprox(ex2)) << "Expeceted:\n " << ex2 << "\nActual:\n " << dm.getCurrentState();

    dm.applyGate("x", {1});
    dm.applyGate("cx", {1, 0});

    MatrixXcd ex3 = MatrixXcd::Zero(4,4);
    ex3(2,2) = 1;

    EXPECT_TRUE(dm.getCurrentState().isApprox(ex3)) << "Expeceted:\n " << ex3 << "\nActual:\n " << dm.getCurrentState();
}

TEST(DensityMatrixTest, GHZStatePreparation) {
    stateVector sv(3);

    DensityMatrix dm = DensityMatrix::fromStateVector(sv);

    dm.applyGate("h", {0});
    dm.applyGate("cx", {0, 1});
    dm.applyGate("cx", {1, 2});

     MatrixXcd expected = MatrixXcd::Zero(8, 8);
    expected(0, 0) = complex<double>(0.5, 0.0);
    expected(0, 7) = complex<double>(0.5, 0.0);
    expected(7, 0) = complex<double>(0.5, 0.0);
    expected(7, 7) = complex<double>(0.5, 0.0);

    EXPECT_TRUE(dm.getCurrentState().isApprox(expected)) << "Expeceted:\n " << expected << "\nActual:\n " << dm.getCurrentState();

    // Purity should remain 1.0 -- no noise applied, state should stay pure
    double purity = (dm.getCurrentState() * dm.getCurrentState()).trace().real();
    EXPECT_NEAR(purity, 1.0, 1e-9);

    // Trace should remain 1.0 -- probability conservation
    double trace = dm.getCurrentState().trace().real();
    EXPECT_NEAR(trace, 1.0, 1e-9);
}