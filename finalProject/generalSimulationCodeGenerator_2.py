import math

def fmt_q(qubits):
    """Formats a single integer or a list of integers into {1, 2, 3} string format."""
    if isinstance(qubits, int):
        return f"{{{qubits}}}"
    return "{" + ",".join(map(str, qubits)) + "}"

n = 10
target = "1010101010"
# Using math.pi for better precision in Grover iteration calculation
iterations = int(math.pi / 4 * math.sqrt(2**n)) 

for i in range(iterations):
    # --- Oracle ---
    for bit_index, bit in enumerate(reversed(target)):
        if bit == '0':
            print(f'simulate_added_gate(qs, {fmt_q(bit_index)}, "X", PI, 0, res, filename);')

    # Phase kickback on ancilla
    print(f'simulate_added_gate(qs, {fmt_q(n-1)}, "H", PI/2, PI/2, res, filename);')
    
    # MCX: combine target (n-1) and controls (0 to n-2) into one list
    mcx_list = [n-1] + list(range(n-1))
    print(f'simulate_added_gate(qs, {fmt_q(mcx_list)}, "MCX", PI, 0, res, filename);')
    
    print(f'simulate_added_gate(qs, {fmt_q(n-1)}, "H", PI/2, PI/2, res, filename);')

    # Uncompute Oracle
    for bit_index, bit in enumerate(reversed(target)):
        if bit == '0':
            print(f'simulate_added_gate(qs, {fmt_q(bit_index)}, "X", PI, 0, res, filename);')

    # --- Diffusion Operator ---
    all_qubits = list(range(n))
    print(f'simulate_added_gate(qs, {fmt_q(all_qubits)}, "H", PI/2, PI/2, res, filename);')
    print(f'simulate_added_gate(qs, {fmt_q(all_qubits)}, "X", PI, 0, res, filename);')

    print(f'simulate_added_gate(qs, {fmt_q(n-1)}, "H", PI/2, PI/2, res, filename);')
    print(f'simulate_added_gate(qs, {fmt_q(mcx_list)}, "MCX", PI, 0, res, filename);')
    print(f'simulate_added_gate(qs, {fmt_q(n-1)}, "H", PI/2, PI/2, res, filename);')

    print(f'simulate_added_gate(qs, {fmt_q(all_qubits)}, "X", PI, 0, res, filename);')
    print(f'simulate_added_gate(qs, {fmt_q(all_qubits)}, "H", PI/2, PI/2, res, filename);') 