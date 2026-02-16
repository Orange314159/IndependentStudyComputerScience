package com.simulation;

import com.core.QuantumComputer;

/**
 * Helper class for building quantum circuits with a (hopefully) good interface.
 */
public class CircuitBuilder {
    private final QuantumComputer quantumComputer;
    private StringBuilder circuitLog;

    /**
     * Create a circuit builder for a quantum computer.
     * @param quantumComputer The quantum computer to build the circuit for
     */
    public CircuitBuilder(QuantumComputer quantumComputer) {
        this.quantumComputer = quantumComputer;
        this.circuitLog = new StringBuilder();
        this.circuitLog.append("Circuit for ").append(quantumComputer.getName()).append(":\n");
    }

    /**
     * Apply Hadamard gate.
     * @param qubitIndex The index for the qubit to apply the gate on
     * @return This builder (to allow for chaining commands)
     */
    public CircuitBuilder hadamard(int qubitIndex) {
        this.quantumComputer.applyHadamard(qubitIndex);
        circuitLog.append(" H(q").append(qubitIndex).append(")\n");
        return this;
    }    
    
    /**
     * Apply Pauli-X gate.
     * @param qubitIndex The index for the qubit to apply the gate on
     * @return This builder (to allow for chaining commands)
     */
    public CircuitBuilder pauliX(int qubitIndex) {
        this.quantumComputer.applyPauliX(qubitIndex);
        circuitLog.append(" X(q").append(qubitIndex).append(")\n");
        return this;
    }

    /**
     * Apply Pauli-Z gate.
     * @param qubitIndex The index for the qubit to apply the gate on
     * @return This builder (to allow for chaining commands)
     */
    public CircuitBuilder pauliZ(int qubitIndex) {
        this.quantumComputer.applyPauliZ(qubitIndex);
        circuitLog.append(" Z(q").append(qubitIndex).append(")\n");
        return this;
    }

    /**
     * Apply CNOT gate.
     * @param qubitIndex The index for the qubit to apply the gate on
     * @return This builder (to allow for chaining commands)
     */
    public CircuitBuilder cnot(int control, int target) {
        this.quantumComputer.applyCNOT(control, target);
        circuitLog.append("  CNOT(q").append(control).append(", q").append(target).append(")\n");
        return this;
    }

    /**
     * Measure a qubit
     * @param qubitIndex The index for the qubit to measure 
     * @return The measurement of the qubit (0 or 1)
     */
    public int measure(int qubitIndex) {
        int result = this.quantumComputer.measure(qubitIndex);
        circuitLog.append("  Measure(q").append(qubitIndex).append(") = ").append(result).append("\n");
        return result;
    }

    /**
     * Creates a bell entangled pair of qubits
     * @param qubit1 First qubit
     * @param qubit2 Second qubit
     * @return This builder (to allow for chaining commands)
     */
    public CircuitBuilder createBellPair(int qubit1, int qubit2) {
        hadamard(qubit1);
        cnot(qubit1, qubit2);
        circuitLog.setLength(circuitLog.length() - 2); // remove the comments that were made by the hadamard and cnot
        circuitLog.append("  Bell Pair(q").append(qubit1).append(", q").append(qubit2).append(")\n");
        return this;
    }

    /**
     * Reset the circuit log
     */
    public void resetLog(){
        circuitLog = new StringBuilder();
        this.circuitLog.append("Circuit for ").append(quantumComputer.getName()).append(":\n");
    }

    /**
     * Get the circuit log
     * @return String representation of the circuit operations
     */
    public String getCircuitLog() {
        return circuitLog.toString();
    }

    /**
     * Prints the circuit log
     */
    public void printLog() {
        System.out.println(circuitLog.toString());
    }

    public QuantumComputer getQuantumComputer() {
        return this.quantumComputer;
    }

}
