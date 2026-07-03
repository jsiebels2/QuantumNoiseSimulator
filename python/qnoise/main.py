from qnoise_cpp import StateVector
from qiskit import QuantumCircuit
from qiskit.quantum_info import Statevector
import numpy as np
from qnoise_cpp import DensityMatrix

# My cpp state vector class
sv = StateVector(2)

# Qiskit state vector for comparison
qc = QuantumCircuit(2)
qc.h(0)  # Apply hadimard gate to qubit 0
qc.h(1)
qiskit_sv = Statevector.from_instruction(qc)

currentState = sv.getCurrentState
print("Initial state:", currentState)
print("Dimensions:", sv.getDimensions)
print("Number of qubits:", sv.getQubits)

gate = np.array([[1/np.sqrt(2), 1/np.sqrt(2)],
                 [1/np.sqrt(2), -1/np.sqrt(2)]], dtype=np.complex128)

sv.applySingleQubitGate(gate, 0)
sv.applySingleQubitGate(gate, 1)

currentState = sv.getCurrentState

dm = DensityMatrix.fromStateVector(sv)

print("Density Matrix from the state vector:\n", dm.getDensityMatrix())

print("My State after applying the hadimard gate to qubit 0:", currentState)
print("Qiskit State after applying the hadimard gate to qubit 0:", qiskit_sv.data)
