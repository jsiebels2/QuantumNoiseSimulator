#include <gtest/gtest.h>
#include "core/channels/AmplitudeDamping/amplitude_damping.cpp"
#include "core/channels/BitPhaseFlips/bit_phase_flips.cpp"
#include "core/channels/DepolarizingNoise/depolarizing_noise.cpp"
#include "core/channels/PhaseDamping/phase_damping.cpp"
#include "stateVector/stateVector.h"
#include "core/DensityMatrix/density_matrix.hpp"

TEST(NoiseChannelTest, TestAmplitudDampingPurity) {
    vector<double> gammaValues = {0.0, 1.0, 0.1, 0.5};

    for(auto& gamma: gammaValues) {
        AmplitudeDamping ch(gamma);
        vector<MatrixXcd> krausOps = ch.getKrausOps();
        Matrix2cd sum = Matrix2cd::Zero();

        for(auto& op : krausOps) {
            sum += op.adjoint() * op;
        }

        EXPECT_TRUE(sum.isApprox(Matrix2cd::Identity(), 1e-10)) << "The test failed for gamma = " << gamma << "." << endl; 
    }
}

TEST(NoiseChannelTest, TestAmplitudeDampingOnExistingState) {
    vector<double> gammaValues = {0.0, 1.0, 0.1, 0.5};
    stateVector sv(1);
    DensityMatrix dm = DensityMatrix::fromStateVector(sv);

    Matrix2cd h;
    h << 1/sqrt(2), 1/sqrt(2),
         1/sqrt(2), -1/sqrt(2);

    Matrix2cd exp1;
    exp1 << 0.5, 0.5,
            0.5, 0.5;

    for(auto& y: gammaValues) {
        AmplitudeDamping ch(y);
        DensityMatrix newMatrix = dm;
        newMatrix.applyGate(h, {0});

        Matrix2cd m = newMatrix.getDensityMatrix();
        MatrixXcd p_squared = m * m;
        double trace = p_squared.trace().real();

        EXPECT_TRUE(m.isApprox(exp1, 1e-10)) 
        << "Density matrix does not match the expected value\n"
        << m << "\n"
        << "Expected:\n" << exp1
        << endl;

        EXPECT_NEAR(trace, 1.0, 1e-10) << "Trace: " << trace << endl;

        ch.apply(newMatrix, {0});

        m = newMatrix.getDensityMatrix();
        p_squared = m * m;
        trace = p_squared.trace().real();

        double s = std::sqrt(1 - y); // √(1-γ)

        Eigen::Matrix2cd expected;
        expected << 0.5 + 0.5 * y, 0.5 * s,
                    0.5 * s, 0.5 - 0.5 * y;

        EXPECT_TRUE(m.isApprox(expected, 1e-10)) 
        << "The kraus application is incorrect for gamma: " << y << ".\n"
        << "Result:\n" << m << "\n"
        << "Expected:\n" << expected << endl;

        EXPECT_TRUE(trace < 1.0) << "Tace: " << trace << endl;
    }
}

TEST(NoiseChannelTest, TestBitPhaseFlipsPurity) {
    vector<double> pValues = {0.0, 1.0, 0.1};

    for(auto& p: pValues) {
        AmplitudeDamping ch(p);
        vector<MatrixXcd> krausOps = ch.getKrausOps();
        Matrix2cd sum = Matrix2cd::Zero();

        for(auto& op : krausOps) {
            sum += op.adjoint() * op;
        }

        EXPECT_TRUE(sum.isApprox(Matrix2cd::Identity(), 1e-10)) << "The test failed for p = " << p << "." << endl; 
    }
}

TEST(NoiseChannelTest, BitAndPhaseFlipOnExistingState) {
    vector<double> pValues = {0.0, 1.0, 0.1, 0.5};
    stateVector sv(1);
    DensityMatrix dm = DensityMatrix::fromStateVector(sv);

    Matrix2cd h;
    h << 1/sqrt(2), 1/sqrt(2),
         1/sqrt(2), -1/sqrt(2);

    Matrix2cd exp0;
    exp0 << 0.5, 0.5,
            0.5, 0.5;

    for(auto& p: pValues) {
        BitPhaseFlips ch(p);
        DensityMatrix newMatrix = dm;
        newMatrix.applyGate(h, {0});

        Matrix2cd m = newMatrix.getDensityMatrix();
        MatrixXcd p_squared = m * m;
        double trace = p_squared.trace().real();

        EXPECT_TRUE(m.isApprox(exp0, 1e-10)) 
        << "Density matrix does not match the expected value\n"
        << m << "\n"
        << "Expected:\n" << exp0
        << endl;

        EXPECT_NEAR(trace, 1.0, 1e-10) << "Trace: " << trace << endl;

        ch.apply(newMatrix, {0});
        
        m = newMatrix.getDensityMatrix();
        p_squared = m * m;
        trace = p_squared.trace().real();

        Matrix2cd exp1;
        exp1 << 0.5, (0.5*(2*p-1)),
                (0.5*(2*p-1)), 0.5;

            EXPECT_TRUE(m.isApprox(exp1, 1e-10))
        << "The kraus application is incorrect for p-value: " << p << ".\n"
        << "Result:\n" << m << "\n"
        << "Expected:\n" << exp1 << endl;

        EXPECT_TRUE(trace < 1) << "Trace: " << trace << endl;
    }
}

TEST(NoiseChannelTest, TestDepolarizingNoisePurity) {
    vector<double> pValues = {0.0, 1.0, 0.1};

    for(auto& p: pValues) {
        AmplitudeDamping ch(p);
        vector<MatrixXcd> krausOps = ch.getKrausOps();
        Matrix2cd sum = Matrix2cd::Zero();

        for(auto& op : krausOps) {
            sum += op.adjoint() * op;
        }

        EXPECT_TRUE(sum.isApprox(Matrix2cd::Identity(), 1e-10)) << "The test failed for p = " << p << "." << endl; 
    }
}

TEST(NoiseChannelTest, TestDepolarizingNoiseOnExistingState) {
    vector<double> pValues = {0.0, 1.0, 0.1, 0.5};
    stateVector sv(1);
    DensityMatrix dm = DensityMatrix::fromStateVector(sv);

    Matrix2cd h;
    h << 1/sqrt(2), 1/sqrt(2),
         1/sqrt(2), -1/sqrt(2);

    Matrix2cd exp1;
    exp1 << 0.5, 0.5,
            0.5, 0.5;

    for(auto& p: pValues) {
        DepolarizingNoise ch(p);
        DensityMatrix newMatrix = dm;
        newMatrix.applyGate(h, {0});

        Matrix2cd m = newMatrix.getDensityMatrix();
        MatrixXcd p_squared = m * m;
        double trace = p_squared.trace().real();

        EXPECT_TRUE(m.isApprox(exp1, 1e-10)) 
        << "Density matrix does not match the expected value\n"
        << m << "\n"
        << "Expected:\n" << exp1
        << endl;

        EXPECT_NEAR(trace, 1.0, 1e-10) << "Trace: " << trace << endl;

        ch.apply(newMatrix, {0});
        
        m = newMatrix.getDensityMatrix();
        p_squared = m * m;
        trace = p_squared.trace().real();

        Matrix2cd exp = m;

        EXPECT_TRUE(m.isApprox(exp, 1e-10))
        << "The kraus application is incorrect for p-value: " << p << ".\n"
        << "Result:\n" << m << "\n"
        << "Expected:\n" << exp << endl;

        EXPECT_TRUE(trace < 1) << "Trace: " << trace << endl;
    }
}

TEST(NoiseChannelTest, TestPhaseDampingPurity) {
    vector<double> gammaValues = {0.0, 1.0, 0.1, 0.5};

    for(auto& gamma: gammaValues) {
        AmplitudeDamping ch(gamma);
        vector<MatrixXcd> krausOps = ch.getKrausOps();
        Matrix2cd sum = Matrix2cd::Zero();

        for(auto& op : krausOps) {
            sum += op.adjoint() * op;
        }

        EXPECT_TRUE(sum.isApprox(Matrix2cd::Identity(), 1e-10)) << "The test failed for gamma = " << gamma << "." << endl; 
    }
}

TEST(NoiseChannelTest, TestPhaseDampingOnExistingState) {
    vector<double> gammaValues = {0.0, 1.0, 0.1, 0.5};
    stateVector sv(1);
    DensityMatrix dm = DensityMatrix::fromStateVector(sv);

    Matrix2cd h;
    h << 1/sqrt(2), 1/sqrt(2),
         1/sqrt(2), -1/sqrt(2);

    Matrix2cd exp1;
    exp1 << 0.5, 0.5,
            0.5, 0.5;

    for(auto& y: gammaValues) {
        PhaseDamping ch(y);
        DensityMatrix newMatrix = dm;
        newMatrix.applyGate(h, {0});

        Matrix2cd m = newMatrix.getDensityMatrix();
        MatrixXcd p_squared = m * m;
        double trace = p_squared.trace().real();

        EXPECT_TRUE(m.isApprox(exp1, 1e-10)) 
        << "Density matrix does not match the expected value\n"
        << m << "\n"
        << "Expected:\n" << exp1
        << endl;

        EXPECT_NEAR(trace, 1.0, 1e-10) << "Trace: " << trace << endl;

        ch.apply(newMatrix, {0});

        m = newMatrix.getDensityMatrix();
        p_squared = m * m;
        trace = p_squared.trace().real();

        double s = std::sqrt(1 - y); // √(1-γ)

        Eigen::Matrix2cd expected;
        expected << 0.5, 0.5 * s,
                    0.5 * s, 0.5;

        EXPECT_TRUE(m.isApprox(expected, 1e-10)) 
        << "The kraus application is incorrect for gamma: " << y << ".\n"
        << "Result:\n" << m << "\n"
        << "Expected:\n" << expected << endl;

        EXPECT_TRUE(trace < 1.0) << "Trace: " << trace << endl;
    }
}

