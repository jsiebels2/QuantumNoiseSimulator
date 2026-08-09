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
  - `NoiseChannles` - a class that allows you to define which noise channel you want to use

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
import qnoise

# Create a 2-qubit circuit
circuit = qnoise.QuantumCircuit(2)

# Add gates
circuit.add_gate("h", [0])       # Hadamard on qubit 0
circuit.add_gate("cx", [0, 1])   # CNOT with control=0, target=1

# Run without noise
result = circuit.run()

# Run with concurrent depolarizing noise (p=0.01)
noisy_result = circuit.run_noisy(noise_type="depolarizing", p=0.01, mode="concurrent")
```

## Roadmap

- Transpiler support
- Expanded gate set
- Benchmarking tools

## License

MIT