# qnoise

A high-performance quantum noise simulation library with a C++ core exposed via Python. qnoise lets you build quantum circuits and simulate them under realistic noise conditions — validated against Qiskit Aer to within 1e-6 error.

## Installation

```bash
pip install qnoisesim
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
- **Mid-circuit measurement** — measure individual qubits at any point; qubits remain usable after measurement
- **Shot-based sampling** — run N executions and collect a measurement histogram
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

# Create a 2-qubit Bell state circuit
circuit = qnoise.QuantumCircuit(2)
circuit.addGate("h", [0])
circuit.addGate("cx", [0, 1])
circuit.measure([0, 1])

# Execute and sample — returns a dict mapping bitstrings to counts
# Unmeasured qubits appear as '-' in the bitstring
counts = circuit.executeCircuit("", "", 0.0)          # no noise, default 1024 shots
counts = circuit.executeCircuit("amplitude-damping", "posterior-noise", 0.1)
counts = circuit.executeCircuit("depolarizing-noise", "concurrent-noise", 0.1, 2048)

print(counts)  # e.g. {"00": 511, "11": 513}
```

### Legacy single-execution API

```python
# Returns the final StateVector or DensityMatrix directly (no sampling)
sv = circuit.executeWithoutNoise()
dm = circuit.executeWithPosteriorNoise("amplitude-damping", 0.1)
dm = circuit.executeConcurrentNoise("depolarizing-noise", 0.1)

print(np.array(sv.getCurrentState()))
print(np.array(dm.getCurrentState()))
```

## Roadmap

- Transpiler support
- Expanded gate set
- Lindblad master equation solver
- Benchmarking tools
- Visualization tools

## License

MIT