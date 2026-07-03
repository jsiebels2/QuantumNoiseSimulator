#include "core/DensityMatrix/density_matrix.hpp"
#include "stateVector/stateVector.h"
#include <gtest/gtest.h>

TEST(DensityMatrixTest, FromStateVector) {
    stateVector sv(1);
    DensityMatrix dm = DensityMatrix::fromStateVector(sv);

    MatrixXcd expected(2, 2);
    expected << complex<double>(1.0, 0.0), complex<double>(0.0, 0.0),
                complex<double>(0.0, 0.0), complex<double>(0.0, 0.0);

    EXPECT_TRUE(dm.getDensityMatrix().isApprox(expected));
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

    EXPECT_TRUE(dm.getDensityMatrix().isApprox(expected));
}