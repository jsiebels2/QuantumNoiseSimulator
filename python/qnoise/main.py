from qnoise_cpp import StateVector
from qiskit import QuantumCircuit
from qiskit.quantum_info import Statevector
import numpy as np

# My cpp state vector class
sv = StateVector(2)

# Qiskit state vector for comparison
qc = QuantumCircuit(2)
qc.x(0)  # Apply X gate to qubit 0
qiskit_sv = Statevector.from_instruction(qc)

currentState = sv.getCurrentState
print("Initial state:", currentState)
print("Dimensions:", sv.getDimensions)
print("Number of qubits:", sv.getQubits)

gate = np.array([[0, 1],
                 [1, 0]], dtype=np.complex128)

sv.applySingleQubitGate(gate, 0)

currentState = sv.getCurrentState

print("My State after applying X gate to qubit 0:", currentState)
print("Qiskit State after applying X gate to qubit 0:", qiskit_sv.data)
