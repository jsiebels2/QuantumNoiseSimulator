import pytest
from qnoise import qnoise_cpp
from qiskit_aer.noise import amplitude_damping_error, phase_damping_error, depolarizing_error, pauli_error, NoiseModel
from qiskit_aer import AerSimulator
from qiskit_aer.library import save_density_matrix
from qiskit import QuantumCircuit, transpile
import numpy as np

def run_qiskit_noise(gate: str, noiseChannel, p: float):
    #Build the circuit
    qc = QuantumCircuit(1)
    getattr(qc, gate)(0)

    #Build the noise mode
    noise_model = NoiseModel()
    channel = build_qiskit_noise_channel(noiseChannel, p)
    noise_model.add_all_qubit_quantum_error(channel, gate)
    
    #simulate
    sim = AerSimulator(method='density_matrix', noise_model=noise_model)
    qc.save_density_matrix()
    result = sim.run(transpile(qc, sim, optimization_level=0)).result()
    return result.data()['density_matrix'].data

def run_qnoise_noise_simulator(gate: str, noiseChannel: str, p: float):
    sv = qnoise_cpp.StateVector(1)
    dm = qnoise_cpp.DensityMatrix.fromStateVector(sv)
    model = build_qnoise_noise_channel(noiseChannel, p)
    dm.applyGate(gate, {0})
    model.apply(dm, {0})
    return dm.getDensityMatrix()

def build_qiskit_noise_channel(channel: str, p: float):
    match channel:
        case "Amplitude Damping":
            return amplitude_damping_error(p)
        case 'Depolarizing Noise':
            return depolarizing_error(p, 1)
        case "Phase Damping":
            return phase_damping_error(p)
        case "Bit Phase Flip":
            return pauli_error([("Y", 1-p), ("I", p)])
        case _:
            raise KeyError("There is not support for " + channel)

def build_qnoise_noise_channel(channel: str, p: float):
    match channel:
        case "Amplitude Damping":
            return qnoise_cpp.AmplitudeDamping(p)
        case 'Depolarizing Noise':
            return qnoise_cpp.DepolarizingNoise(p)
        case "Phase Damping":
            return qnoise_cpp.PhaseDamping(p)
        case "Bit Phase Flip":
            return qnoise_cpp.BitPhaseFlip(p)
        case _:
            raise KeyError("There is not support for " + channel) 
        
@pytest.mark.parametrize("gate", ["id", "x", "y", "z", "h"])
@pytest.mark.parametrize("p", [0.0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0])       
def test_amplitude_damping_against_qiskit(gate: str, p: float):
    qiskit_noise = run_qiskit_noise(gate, "Amplitude Damping", p)
    qnoise_noise = run_qnoise_noise_simulator(gate, "Amplitude Damping", p)
    assert np.allclose(qiskit_noise, qnoise_noise, rtol=.00001)

@pytest.mark.parametrize("gate", ["id", "x", "y", "z", "h"])
@pytest.mark.parametrize("p", [0.0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0])       
def test_phase_damping_against_qiskit(gate: str, p: float):
    qiskit_noise = run_qiskit_noise(gate, "Phase Damping", p)
    qnoise_noise = run_qnoise_noise_simulator(gate, "Phase Damping", p)
    assert np.allclose(qiskit_noise, qnoise_noise, rtol=.00001)

@pytest.mark.parametrize("gate", ["id", "x", "y", "z", "h"])
@pytest.mark.parametrize("p", [0.0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0])       
def test_depolarizing_noise_against_qiskit(gate: str, p: float):
    qiskit_noise = run_qiskit_noise(gate, "Depolarizing Noise", p)
    qnoise_noise = run_qnoise_noise_simulator(gate, "Depolarizing Noise", p)
    assert np.allclose(qiskit_noise, qnoise_noise, rtol=.00001)

@pytest.mark.parametrize("gate", ["id", "x", "y", "z", "h"])
@pytest.mark.parametrize("p", [0.0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0])       
def test_bit_phase_flip_against_qiskit(gate: str, p: float):
    qiskit_noise = run_qiskit_noise(gate, "Bit Phase Flip", p)
    qnoise_noise = run_qnoise_noise_simulator(gate, "Bit Phase Flip", p)
    assert np.allclose(qiskit_noise, qnoise_noise, rtol=.00001)
