import pytest
from qnoise import qnoise_cpp
from qiskit_aer.noise import amplitude_damping_error, phase_damping_error, depolarizing_error, pauli_error, NoiseModel
from qiskit_aer import AerSimulator
from qiskit_aer.library import save_density_matrix
from qiskit import QuantumCircuit, transpile
import numpy as np

def construct_qiskit_circuit(numQubits: int, circuit, noiseChannel: str, p: float):
    qc = QuantumCircuit(numQubits)
    
    for gate, qubits in circuit:
        match gate:
            case "x":
                qc.x(qubits)
            case "y":
                qc.y(qubits)
            case "z":
                qc.z(qubits)
            case "h":
                qc.h(qubits)
            case "id":
                qc.id(qubits)
            case "cx":
                qc.cx(*qubits)
            case "cz":
                qc.cz(*qubits)
            case "sw":
                qc.swap(*qubits)
            case _:
                print("Unknown Gate")

    #Build the noise model
    noise_model = NoiseModel()
    channel = build_qiskit_noise_channel(noiseChannel, p)
    noise_model.add_all_qubit_quantum_error(channel, gate)
    
    sim = AerSimulator(method='density_matrix', noise_model=noise_model)
    qc.save_density_matrix()
    result = sim.run(transpile(qc, sim, optimization_level=0)).result()
    return result.data()['density_matrix'].data

def construct_qnoise_circuit(numQubits: int, circuit, noiseChannel: str, p: float):
    qc = qnoise_cpp.QuantumCircuit(numQubits)

    for gate, qubits in circuit:
        if(is_multi_Qubit_Circuit(gate)):
            qc.addGate(gate, [*qubits])
        else:
            qc.addGate(gate, [qubits])

    return qc.executeConcurrentNoise(noiseChannel, p)

def is_multi_Qubit_Circuit(gate: str):
    match gate:
        case "cx":
            return True
        case "cz":
            return True
        case "sw":
            return True
        case _:
            return False


def densitymatrices_match(dm_qiskit, dm_qnoise: qnoise_cpp.StateVector) -> bool:
    a = np.array(dm_qiskit)
    b = np.array(dm_qnoise.getCurrentState)
    return np.allclose(a, b, atol=1e-6)

def build_qiskit_noise_channel(channel: str, p: float):
    match channel:
        case "amplitude-damping":
            return amplitude_damping_error(p)
        case 'depolarizing-noise':
            return depolarizing_error(p, 1)
        case "phase-damping":
            return phase_damping_error(p)
        case "bit-phase-flips":
            return pauli_error([("Y", 1-p), ("I", p)])
        case _:
            raise KeyError("There is not support for " + channel)
        
def build_qnoise_noise_channel(channel: str, p: float):
    match channel:
        case "amplitude-damping":
            return qnoise_cpp.AmplitudeDamping(p)
        case 'depolarizing-noise':
            return qnoise_cpp.DepolarizingNoise(p)
        case "phase-damping":
            return qnoise_cpp.PhaseDamping(p)
        case "bit-phase-flips":
            return qnoise_cpp.BitPhaseFlip(p)
        case _:
            raise KeyError("There is not support for " + channel) 


@pytest.mark.parametrize("num_qubits, circuit, noiseChannel, p", [
    (2, [("cx", (0, 1))], "amplitude-damping", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (2, [("cx", (0, 1))], "phase-damping", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),  # control off, no flip
    (2, [("cx", (0, 1))], "depolarizing-noise", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (2, [("cx", (0, 1))], "bit-phase-flips", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (2, [("x", 0), ("cx", (0, 1))], "amplitude-damping", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]), # control on, flip target
    (2, [("x", 0), ("cx", (0, 1))], "phase-damping", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (2, [("x", 0), ("cx", (0, 1))], "depolarizing-noise", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (2, [("x", 0), ("cx", (0, 1))], "bit-phase-flips", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (2, [("h", 0), ("h", 1), ("cz", (0, 1))], "amplitude-damping", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),   # cz in superposition
    (2, [("h", 0), ("h", 1), ("cz", (0, 1))], "phase-damping", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (2, [("h", 0), ("h", 1), ("cz", (0, 1))], "depolarizing-noise", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (2, [("h", 0), ("h", 1), ("cz", (0, 1))], "bit-phase-flips", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (2, [("x", 0), ("sw", (0, 1))], "amplitude-damping", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),              # swap flips qubits
    (2, [("x", 0), ("sw", (0, 1))], "phase-damping", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (2, [("x", 0), ("sw", (0, 1))], "depolarizing-noise", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (2, [("x", 0), ("sw", (0, 1))], "bit-phase-flips", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
])
def test_two_qubit_gates(num_qubits, circuit, noiseChannel, p):
    for y in p:
        assert densitymatrices_match(
            construct_qiskit_circuit(num_qubits, circuit, noiseChannel, y),
            construct_qnoise_circuit(num_qubits, circuit, noiseChannel, y),
        )



@pytest.mark.parametrize("num_qubits, circuit, noiseChannel, p", [
    (2, [("h", 0), ("cx", (0, 1))], "amplitude-damping", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),  # Bell state
    (2, [("h", 0), ("cx", (0, 1))], "phase-damping", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (2, [("h", 0), ("cx", (0, 1))], "depolarizing-noise", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (2, [("h", 0), ("cx", (0, 1))], "bit-phase-flips", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (3, [("h", 0), ("cx", (0, 1)), ("cx", (0, 2))], "amplitude-damping", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),  # GHZ state
    (3, [("h", 0), ("cx", (0, 1)), ("cx", (0, 2))], "phase-damping", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (3, [("h", 0), ("cx", (0, 1)), ("cx", (0, 2))], "depolarizing-noise", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (3, [("h", 0), ("cx", (0, 1)), ("cx", (0, 2))], "bit-phase-flips", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (1, [("x", 0), ("x", 0)], "amplitude-damping", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),    # X·X = I
    (1, [("x", 0), ("x", 0)], "phase-damping", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (1, [("x", 0), ("x", 0)], "depolarizing-noise", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (1, [("x", 0), ("x", 0)], "bit-phase-flips", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (1, [("h", 0), ("h", 0)], "amplitude-damping", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),  # H·H = I
    (1, [("h", 0), ("h", 0)], "phase-damping", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (1, [("h", 0), ("h", 0)], "depolarizing-noise", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (1, [("h", 0), ("h", 0)], "bit-phase-flips", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (3, [("h", 0), ("h", 1), ("x", 2), ("cz", (0, 1)), ("cx", (1, 2)), ("z", 0)], "amplitude-damping", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (3, [("h", 0), ("h", 1), ("x", 2), ("cz", (0, 1)), ("cx", (1, 2)), ("z", 0)], "phase-damping", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (3, [("h", 0), ("h", 1), ("x", 2), ("cz", (0, 1)), ("cx", (1, 2)), ("z", 0)], "depolarizing-noise", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
    (3, [("h", 0), ("h", 1), ("x", 2), ("cz", (0, 1)), ("cx", (1, 2)), ("z", 0)], "bit-phase-flips", [ 0.1, 0.3, 0.5, 0.7, 0.9, 1.0]),
])
def test_composite_circuits(num_qubits, circuit, noiseChannel, p):
    for y in p:
        assert densitymatrices_match(
            construct_qiskit_circuit(num_qubits, circuit, noiseChannel, y),
            construct_qnoise_circuit(num_qubits, circuit, noiseChannel, y),
        )

