# qnoise
 
A high-performance quantum noise simulation library with a C++ core exposed via Python. qnoise lets you build quantum circuits and simulate them under realistic noise conditions — validated against Qiskit Aer to within 1e-6 error.
 
## Installation
 
```bash
pip install qnoise
```
 
> **Note:** Installation compiles a C++ extension. You'll need a C++ compiler (Xcode CLT on macOS, `build-essential` on Linux, MSVC on Windows).
 
## Features
 
- **Three simulation modes**
  - No noise — clean circuit execution
  - End-of-circuit noise — noise applied once after all gates
  - Concurrent noise — noise applied at each gate application
- **Four noise channels**
  - Depolarizing noise
  - Amplitude damping
  - Phase damping
  - Bit-phase flips
- **Three core classes**
  - `QuantumCircuit` — build and run circuits
  - `StateVector` — pure state representation and operations
  - `DensityMatrix` — mixed state representation and operations
- **Validated against Qiskit Aer** across multi-qubit circuits to within 1e-6 error
## Supported Gates
 
| Key | Gate |
|-----|------|
| `"x"` | Pauli-X |
| `"y"` | Pauli-Y |
| `"z"` | Pauli-Z |
| `"h"` | Hadamard |
| `"id"` | Identity |
| `"cx"` | CNOT |
| `"cz"` | CZ |
| `"sw"` | SWAP |
 
## Quick Start
 
```python
from qnoise import qnoise
import numpy as np
 
# Create a 2-qubit circuit (Bell state)
circuit = qnoise.QuantumCircuit(2)
 
# Build the circuit — Hadamard on qubit 0, then CNOT with control=0, target=1
circuit.addGate("h", [0])
circuit.addGate("cx", [0, 1])
 
# Execute without noise — returns a StateVector representing the ideal output
resultWithNoNoise = circuit.executeWithoutNoise()

#Execute with noise applied at the end
resultWithPosteriorNoise = curcuit.executeWithPosteriorNoise("amplitude-damping", 0.1)
 
# Execute with concurrent noise — noise is applied after each gate application.
# The second argument is the noise strength (gamma for damping channels, p for others).
amplitudeDamping = circuit.executeConcurrentNoise("amplitude-damping", 0.1)
depolarizing     = circuit.executeConcurrentNoise("depolarizing-noise", 0.1)
bitPhaseFlips    = circuit.executeConcurrentNoise("bit-phase-flips", 0.1)
phaseDamping     = circuit.executeConcurrentNoise("phase-damping", 0.1)
 
# getCurrentState() returns the underlying state as a list — wrap in np.array for easy use
print(np.array(resultWithNoNoise.getCurrentState()))
print(np.array(amplitudeDamping.getCurrentState()))
print(np.array(depolarizing.getCurrentState()))
print(np.array(bitPhaseFlips.getCurrentState()))
print(np.array(phaseDamping.getCurrentState()))
```
 
## Roadmap
 
- Transpiler support
- Expanded gate set
- Benchmarking tools
- Visualization tools
## License
 
MIT