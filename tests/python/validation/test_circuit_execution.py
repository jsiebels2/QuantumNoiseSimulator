import pytest
from qnoise import qnoise
from qiskit import QuantumCircuit
from qiskit.quantum_info import Statevector
import numpy as np

def construct_qiskit_circuit(numQubits: int, circuit: dict):
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
    
    sv = Statevector.from_instruction(qc)
    return sv

def construct_qnoise_circuit(numQubits: int, circuit: dict):
    qc = qnoise.QuantumCircuit(numQubits)

    for gate, qubits in circuit:
        if(is_multi_Qubit_Circuit(gate)):
            qc.addGate(gate, [*qubits])
        else:
            qc.addGate(gate, [qubits])

    return qc.executeWithoutNoise()

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


def statevectors_match(sv_qiskit, sv_qnoise: qnoise.StateVector) -> bool:
    a = np.array(sv_qiskit)
    b = np.array(sv_qnoise.getCurrentState)
    idx = np.flatnonzero(np.abs(a) > 1e-10)
    if len(idx) == 0:
        return np.allclose(a, b)
    phase = a[idx[0]] / b[idx[0]]
    return np.allclose(a, b * phase, atol=1e-6)


@pytest.mark.parametrize("num_qubits, circuit", [
    (1, [("x", 0)]),
    (1, [("y", 0)]),
    (1, [("z", 0)]),
    (1, [("h", 0)]),
    (1, [("id", 0)]),
])
def test_single_qubit_gates(num_qubits, circuit):
    assert statevectors_match(
        construct_qiskit_circuit(num_qubits, circuit),
        construct_qnoise_circuit(num_qubits, circuit),
    )


@pytest.mark.parametrize("num_qubits, circuit", [
    (2, [("cx", (0, 1))]),                        # control off, no flip
    (2, [("x", 0), ("cx", (0, 1))]),              # control on, flip target
    (2, [("h", 0), ("h", 1), ("cz", (0, 1))]),   # cz in superposition
    (2, [("x", 0), ("sw", (0, 1))]),              # swap flips qubits
])
def test_two_qubit_gates(num_qubits, circuit):
    assert statevectors_match(
        construct_qiskit_circuit(num_qubits, circuit),
        construct_qnoise_circuit(num_qubits, circuit),
    )


@pytest.mark.parametrize("num_qubits, circuit", [
    (2, [("h", 0), ("cx", (0, 1))]),                            # Bell state
    (3, [("h", 0), ("cx", (0, 1)), ("cx", (0, 2))]),           # GHZ state
    (1, [("x", 0), ("x", 0)]),                                  # X·X = I
    (1, [("h", 0), ("h", 0)]),                                  # H·H = I
    (3, [("h", 0), ("h", 1), ("x", 2), ("cz", (0, 1)), ("cx", (1, 2)), ("z", 0)]),
])
def test_composite_circuits(num_qubits, circuit):
    assert statevectors_match(
        construct_qiskit_circuit(num_qubits, circuit),
        construct_qnoise_circuit(num_qubits, circuit),
    )


