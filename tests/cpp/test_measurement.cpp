#include "core/QuantumCircuit/QuantumCircuit.hpp"
#include <Eigen/Dense>
#include <gtest/gtest.h>

TEST(MeasurementTest, CollapseKnownState) {
    QuantumCircuit qc = QuantumCircuit(1);
    qc.addGate("x", {0});        // |1⟩, guaranteed measure = 1
    qc.addGate("measure", {0});
    qc.executeWithoutNoise();

    Eigen::VectorXcd sv = qc.getCurrentStateVector();
    EXPECT_NEAR(std::abs(sv[0]), 0.0, 1e-9);  // |0⟩ amplitude = 0
    EXPECT_NEAR(std::abs(sv[1]), 1.0, 1e-9);  // |1⟩ amplitude = 1
}

TEST(MeasurementTest, NormPreserved) {
    QuantumCircuit qc = QuantumCircuit(2);
    qc.addGate("h", {0});
    qc.addGate("measure", {0});
    qc.executeWithoutNoise();

    Eigen::VectorXcd sv = qc.getCurrentStateVector();
    double norm = sv.norm();
    EXPECT_NEAR(norm, 1.0, 1e-9);
}

TEST(MeasurementTest, Idempotent) {
    QuantumCircuit qc = QuantumCircuit(1);
    qc.addGate("h", {0});
    qc.addGate("measure", {0});
    qc.addGate("measure", {0});  // second measure shouldn't change state
    qc.executeWithoutNoise();

    Eigen::VectorXcd sv = qc.getCurrentStateVector();
    // One amplitude must be exactly 0, other exactly 1
    EXPECT_TRUE(
        (std::abs(sv[0]) < 1e-9 && std::abs(std::abs(sv[1]) - 1.0) < 1e-9) ||
        (std::abs(sv[1]) < 1e-9 && std::abs(std::abs(sv[0]) - 1.0) < 1e-9)
    );
}

TEST(MeasurementTest, MeasurementStatistics) {
    int zeros = 0, ones = 0;
    for(int i = 0; i < 1000; i++) {
        QuantumCircuit qc = QuantumCircuit(1);
        qc.addGate("h", {0});
        qc.addGate("measure", {0});
        qc.executeWithoutNoise();

        Eigen::VectorXcd sv = qc.getCurrentStateVector();
        if(std::abs(sv[0]) > 0.5) zeros++;
        else ones++;
    }
    // Expect roughly 50/50, within 10%
    EXPECT_GT(zeros, 400);
    EXPECT_GT(ones, 400);
}

TEST(MeasurementTest, PosteriorNoiseCollapseKnownState) {
    QuantumCircuit qc = QuantumCircuit(1);
    qc.addGate("x", {0});        // |1⟩, guaranteed measure = 1
    qc.addGate("measure", {0});
    qc.executeWithPosteriorNoise("depolarizing-noise", 0);

    cout << "cleanly ran execution" << endl; //debug

    Eigen::VectorXcd sv = qc.getCurrentStateVector();
    cout << sv;
    EXPECT_NEAR(std::abs(sv[0]), 0.0, 1e-9);  // |0⟩ amplitude = 0
    EXPECT_NEAR(std::abs(sv[1]), 1.0, 1e-9);  // |1⟩ amplitude = 1
}

TEST(MeasurementTest, PosteriorNoiseNormPreserved) {
    QuantumCircuit qc = QuantumCircuit(2);
    qc.addGate("h", {0});
    qc.addGate("measure", {0});
    qc.executeWithPosteriorNoise("depolarizing-noise", 0);

    Eigen::VectorXcd sv = qc.getCurrentStateVector();
    double norm = sv.norm();
    EXPECT_NEAR(norm, 1.0, 1e-9);
}

TEST(MeasurementTest, PosteriorNoiseIdempotent) {
    QuantumCircuit qc = QuantumCircuit(1);
    qc.addGate("h", {0});
    qc.addGate("measure", {0});
    qc.addGate("measure", {0});  // second measure shouldn't change state
    qc.executeWithPosteriorNoise("depolarizing-noise", 0);

    Eigen::VectorXcd sv = qc.getCurrentStateVector();
    // One amplitude must be exactly 0, other exactly 1
    EXPECT_TRUE(
        (std::abs(sv[0]) < 1e-9 && std::abs(std::abs(sv[1]) - 1.0) < 1e-9) ||
        (std::abs(sv[1]) < 1e-9 && std::abs(std::abs(sv[0]) - 1.0) < 1e-9)
    );
}

TEST(MeasurementTest, PosteriorNoiseMeasurementStatistics) {
    int zeros = 0, ones = 0;
    for(int i = 0; i < 1000; i++) {
        QuantumCircuit qc = QuantumCircuit(1);
        qc.addGate("h", {0});
        qc.addGate("measure", {0});
        qc.executeWithPosteriorNoise("depolarizing-noise", 0);

        Eigen::VectorXcd sv = qc.getCurrentStateVector();
        if(std::abs(sv[0]) > 0.5) zeros++;
        else ones++;
    }
    // Expect roughly 50/50, within 10%
    EXPECT_GT(zeros, 400);
    EXPECT_GT(ones, 400);
}
