import math

n=10
target = "1010101010"
itterations = int(3.14159 / 4  * math.sqrt(2**n)) 

for i in range(itterations):
  for bit_index, bit in enumerate(reversed(target)):
    if bit == '0':
    #   qc.x(bit_index)
      # print(f"Applied X gate to bit index {bit_index} because target bit is 0")
      print(f"simulate_added_gate(qs, {bit_index}, \"X\", PI, 0, res, filename);")

#   qc.h(n-1)
  # print(f"Applied H gate to ancilla qubit at index {n-1}")
  print(f"simulate_added_gate(qs, {n-1}, \"H\", PI/2, PI/2, res, filename);")
#   qc.mcx(list(range(n-1)), n-1)
  # print(f"Applied multi-controlled X gate with control qubits {list(range(n-1))} and target qubit {n-1}")
  print(f"simulate_added_gate(qs, {n-1}{list(range(n-1))}, \"MCX\", PI, 0, res, filename);")
#   qc.h(n-1)
  # print(f"Applied H gate to ancilla qubit at index {n-1} again")
  print(f"simulate_added_gate(qs, {n-1}, \"H\", PI/2, PI/2, res, filename);")

  for bit_index, bit in enumerate(reversed(target)):
    if bit == '0':
    #   qc.x(bit_index)
      # print(f"Applied X gate to bit index {bit_index} because target bit is 0")
      print(f"simulate_added_gate(qs, {bit_index}, \"X\", PI, 0, res, filename);")



#   qc.h(range(n))
  # print(f"Applied H gate to all qubits in the register")
  print(f"simulate_added_gate(qs, {list(range(n))}, \"H\", PI/2, PI/2, res, filename);")
#   qc.x(range(n))
  # print(f"Applied X gate to all qubits in the register")
  print(f"simulate_added_gate(qs, {list(range(n))}, \"X\", PI, 0, res, filename);")

#   qc.h(n-1)
  # print(f"Applied H gate to ancilla qubit at index {n-1}")
  print(f"simulate_added_gate(qs, {n-1}, \"H\", PI/2, PI/2, res, filename);")
#   qc.mcx(list(range(n-1)), n-1)
  # print(f"Applied multi-controlled X gate with control qubits {list(range(n-1))} and target qubit {n-1}")
  print(f"simulate_added_gate(qs, {n-1}{list(range(n-1))}, \"MCX\", PI, 0, res, filename);")

#   qc.h(n-1)
  # print(f"Applied H gate to ancilla qubit at index {n-1} again")
  print(f"simulate_added_gate(qs, {n-1}, \"H\", PI/2, PI/2, res, filename);")

#   qc.x(range(n))
  # print(f"Applied X gate to all qubits in the register again")
  print(f"simulate_added_gate(qs, {list(range(n))}, \"X\", PI, 0, res, filename);")
#   qc.h(range(n))
  # print(f"Applied H gate to all qubits in the register again")
  print(f"simulate_added_gate(qs, {list(range(n))}, \"H\", PI/2, PI/2, res, filename);")

