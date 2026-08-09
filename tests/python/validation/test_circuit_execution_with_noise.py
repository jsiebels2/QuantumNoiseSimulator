import pytest
from qnoise import qnoise_cpp
from qiskit_aer.noise import amplitude_damping_error, phase_damping_error, depolarizing_error, pauli_error, NoiseModel
from qiskit_aer import AerSimulator
from qiskit_aer.library import save_density_matrix
from qiskit_aer.noise import kraus_error
from qiskit.quantum_info import Kraus
from qiskit import QuantumCircuit, transpile
import numpy as np

def construct_qiskit_circuit(numQubits: int, circuit, noiseChannel: str, p: float):
    qc = QuantumCircuit(numQubits)
    single_qubit_gates_used = set()
    two_qubit_gates_used = set()

    for gate, qubits in circuit:
        match gate:
            case "x":
                qc.x(qubits)
                single_qubit_gates_used.add("x")
            case "y":
                qc.y(qubits)
                single_qubit_gates_used.add("y")
            case "z":
                qc.z(qubits)
                single_qubit_gates_used.add("z")
            case "h":
                qc.h(qubits)
                single_qubit_gates_used.add("h")
            case "id":
                qc.id(qubits)
                single_qubit_gates_used.add("id")
            case "cx":
                qc.cx(*qubits)
                two_qubit_gates_used.add("cx")
            case "cz":
                qc.cz(*qubits)
                two_qubit_gates_used.add("cz")
            case "sw":
                qc.swap(*qubits)
                two_qubit_gates_used.add("swap")

    noise_model = NoiseModel()
    if single_qubit_gates_used:
        single_channel = build_qiskit_noise_channel(noiseChannel, p)
        noise_model.add_all_qubit_quantum_error(single_channel, list(single_qubit_gates_used))
    if two_qubit_gates_used:
        multi_channel = construct_multi_qubit_noise_channel(noiseChannel, p)
        noise_model.add_all_qubit_quantum_error(multi_channel, list(two_qubit_gates_used))

    sim = AerSimulator(method='density_matrix', noise_model=noise_model)
    qc.save_density_matrix()
    result = sim.run(transpile(qc, sim, optimization_level=0)).result()
    return result.data()['density_matrix'].data


def construct_multi_qubit_noise_channel(noiseChannel, p):
    single = build_qiskit_noise_channel(noiseChannel, p)
    kraus_ops = Kraus(single.to_quantumchannel()).data
    joint = [np.kron(Ki, Kj) for Ki in kraus_ops for Kj in kraus_ops]
    return kraus_error(joint)

def construct_qnoise_circuit(numQubits: int, circuit, noiseChannel: str, p: float):
    qc = qnoise_cpp.QuantumCircuit(numQubits)

    for gate, qubits in circuit:
        if(is_multi_Qubit_Circuit(gate)):
            qc.addGate(gate, [*qubits])
        else:
            qc.addGate(gate, [qubits])
    dm = qc.executeConcurrentNoise(noiseChannel, p)
    return np.array(dm.getCurrentState())

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


def densitymatrices_match(dm_qiskit, dm_qnoise) -> bool:
    return np.allclose(dm_qiskit, dm_qnoise, atol=1e-6)

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


@pytest.mark.parametrize("p", [ 0.0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0])
@pytest.mark.parametrize("num_qubits, circuit, noiseChannel", [
    (2, [("cx", (0, 1))], "amplitude-damping"),
    (2, [("cx", (0, 1))], "phase-damping"),  # control off, no flip
    (2, [("cx", (0, 1))], "depolarizing-noise"),
    (2, [("cx", (0, 1))], "bit-phase-flips"),
    (2, [("x", 0), ("cx", (0, 1))], "amplitude-damping"), # control on, flip target
    (2, [("x", 0), ("cx", (0, 1))], "phase-damping"),
    (2, [("x", 0), ("cx", (0, 1))], "depolarizing-noise"),
    (2, [("x", 0), ("cx", (0, 1))], "bit-phase-flips"),
    (2, [("h", 0), ("h", 1), ("cz", (0, 1))], "amplitude-damping"),   # cz in superposition
    (2, [("h", 0), ("h", 1), ("cz", (0, 1))], "phase-damping"),
    (2, [("h", 0), ("h", 1), ("cz", (0, 1))], "depolarizing-noise"),
    (2, [("h", 0), ("h", 1), ("cz", (0, 1))], "bit-phase-flips"),
    (2, [("x", 0), ("sw", (0, 1))], "amplitude-damping"),              # swap flips qubits
    (2, [("x", 0), ("sw", (0, 1))], "phase-damping"),
    (2, [("x", 0), ("sw", (0, 1))], "depolarizing-noise"),
    (2, [("x", 0), ("sw", (0, 1))], "bit-phase-flips"),
])
def test_two_qubit_gates(num_qubits, circuit, noiseChannel, p):
    assert densitymatrices_match(
        construct_qiskit_circuit(num_qubits, circuit, noiseChannel, p),
        construct_qnoise_circuit(num_qubits, circuit, noiseChannel, p),
    )



@pytest.mark.parametrize("p", [ 0.0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0])
@pytest.mark.parametrize("num_qubits, circuit, noiseChannel", [
    (2, [("h", 0), ("cx", (0, 1))], "amplitude-damping"),  # Bell state
    (2, [("h", 0), ("cx", (0, 1))], "phase-damping"),
    (2, [("h", 0), ("cx", (0, 1))], "depolarizing-noise"),
    (2, [("h", 0), ("cx", (0, 1))], "bit-phase-flips"),
    (3, [("h", 0), ("cx", (0, 1)), ("cx", (0, 2))], "amplitude-damping"),  # GHZ state
    (3, [("h", 0), ("cx", (0, 1)), ("cx", (0, 2))], "phase-damping"),
    (3, [("h", 0), ("cx", (0, 1)), ("cx", (0, 2))], "depolarizing-noise"),
    (3, [("h", 0), ("cx", (0, 1)), ("cx", (0, 2))], "bit-phase-flips"),
    (1, [("x", 0), ("x", 0)], "amplitude-damping"),    # X·X = I
    (1, [("x", 0), ("x", 0)], "phase-damping"),
    (1, [("x", 0), ("x", 0)], "depolarizing-noise"),
    (1, [("x", 0), ("x", 0)], "bit-phase-flips"),
    (1, [("h", 0), ("h", 0)], "amplitude-damping"),  # H·H = I
    (1, [("h", 0), ("h", 0)], "phase-damping"),
    (1, [("h", 0), ("h", 0)], "depolarizing-noise"),
    (1, [("h", 0), ("h", 0)], "bit-phase-flips"),
    (3, [("h", 0), ("h", 1), ("x", 2), ("cz", (0, 1)), ("cx", (1, 2)), ("z", 0)], "amplitude-damping"),
    (3, [("h", 0), ("h", 1), ("x", 2), ("cz", (0, 1)), ("cx", (1, 2)), ("z", 0)], "phase-damping"),
    (3, [("h", 0), ("h", 1), ("x", 2), ("cz", (0, 1)), ("cx", (1, 2)), ("z", 0)], "depolarizing-noise"),
    (3, [("h", 0), ("h", 1), ("x", 2), ("cz", (0, 1)), ("cx", (1, 2)), ("z", 0)], "bit-phase-flips"),
])
def test_composite_circuits(num_qubits, circuit, noiseChannel, p):
    assert densitymatrices_match(
        construct_qiskit_circuit(num_qubits, circuit, noiseChannel, p),
        construct_qnoise_circuit(num_qubits, circuit, noiseChannel, p),
    )

