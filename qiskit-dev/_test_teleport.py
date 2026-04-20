import numpy as np
from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator

theta = 1.2
qc = QuantumCircuit(3, 3)
qc.h(1)
qc.cx(1, 2)
qc.ry(theta, 0)
qc.cx(0, 1)
qc.h(0)
qc.barrier()
qc.measure([0, 1], [0, 1])
with qc.if_test((qc.clbits[0], True)):
    qc.z(2)
with qc.if_test((qc.clbits[1], True)):
    qc.x(2)
qc.barrier()
qc.measure(2, 2)

sim = AerSimulator()
r = sim.run(qc, shots=4000).result()
print("counts", r.get_counts())

# reference: P(|0>) for Ry(theta)|0>
p0 = float(np.cos(theta / 2) ** 2)
print("expected P(0) on Bob", p0)
