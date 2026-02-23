import numpy as np
from qiskit import *
from qiskit_aer import *

circ = QuantumCircuit(2, 2)
circ.h(0)
circ.cx(0, 1)
circ.measure([0, 1], [0, 1])
circ.draw(output='mpl')

backend_sim = Aer.get_backend('qasm_simulator')

t_circ = transpile(circ, backend_sim)
job_sim = backend_sim.run(t_circ, shots=102400)

result_sim = job_sim.result()

counts = result_sim.get_counts(circ)
print(counts)
