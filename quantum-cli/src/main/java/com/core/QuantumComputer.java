package com.core;

import java.util.Arrays;
import java.util.Random;


/**
 * Represents a quantum computer that can contain quantum circuits. Additionally, it simulates qubits and quantum gate operations.
 */
public class QuantumComputer {
    private String name;
    private final int numQubits;
    private Complex[] state;
    private final Random random;
    // private Map<String, Integer> measurementResults;

    /**
     * Initialize a quantum computer with a given number of qubits.
     * @param name The name of this quantum computer
     * @param numberOfQubits The number of qubits
     */
    public QuantumComputer(String name, int numberOfQubits){
        this.name = name;
        this.numQubits = numberOfQubits;
        this.random = new Random();
        // this.measurementResults = new HashMap<>();
        initialize(numberOfQubits);
    }

    /**
     * Initialize all qubits to state |0> 
     * @param numberOfQubits The number of qubits to initialize
     */
    public final void initialize(int numberOfQubits){
        int stateSize = 1 << numberOfQubits; // implements the use of the left shift for speed up (equivalent to 2^{numberOfQubits})
        this.state = new Complex[stateSize];

        // initialize each complex number
        state[0] = new Complex(1,0);
        for (int i = 1; i < stateSize; i++){
            state[i] = new Complex(0,0);
        }
    }

    /**
     * Validate qubit index is within bounds.
     * @param qubitIndex The index to validate
     * @throws IllegalArgumentException
     */
    private void validateQubitIndex(int qubitIndex){
        if (qubitIndex < 0 || qubitIndex >= numQubits){
            throw new IllegalArgumentException("Qubit index out of bounds: " + qubitIndex);
        }
    }

    /**
     * Apply a Hadamard gate to a qubit.
     * @param qubitIndex The index of the qubit to apply the gate to
     */
    public void applyHadamard(int qubitIndex){
        validateQubitIndex(qubitIndex);
        int stateSize = 1 << numQubits;
        Complex[] newState = new Complex[stateSize];

        for (int i = 0; i < stateSize; i++){
            newState[i] = new Complex(0,0);
        }

        // Apply Hadamard
        double scaleFactor = 1.0 / Math.sqrt(2);

        // Mask out qubits at position qubitIndex
        int mask = 1 << qubitIndex;

        for (int i = 0; i < stateSize; i++){
            // Apply H to each qubit depending on whether it is 1 or 0
            if ((i & mask) == 0){
                newState[i] = newState[i].add(state[i].multiply(scaleFactor));
                newState[i | mask] = newState[i | mask].add(state[i].multiply(scaleFactor));
            } else {
                newState[i ^ mask] = newState[i ^ mask].add(state[i].multiply(scaleFactor));
                newState[i] = newState[i].add(state[i].multiply(-scaleFactor));
            }
        }
        this.state = newState;
    }

    /**
     * Apply a Pauli-X gate to a qubit
     * @param qubitIndex The index of the qubit to apply the gate to
     */
    public void applyPauliX(int qubitIndex) {
        validateQubitIndex(qubitIndex);

        int stateSize = 1 << numQubits;
        Complex[] newState = new Complex[stateSize];

        for (int i = 0; i < stateSize; i++) {
            newState[i] = new Complex(0,0);
        }

        int mask = 1 << qubitIndex;

        for (int i = 0; i < stateSize; i++){
            newState[i ^ mask] = state[i];
        }

        state = newState;
    }

    /**
     * Apply a Pauli-Z gate to a qubit
     * @param qubitIndex The index of the qubit to apply the gate to
     */
    public void applyPauliZ(int qubitIndex) {
        validateQubitIndex(qubitIndex);

        int mask = 1 << qubitIndex;

        for (int i = 0; i < state.length; i ++){
            if ((i & mask) != 0){
                this.state[i] = this.state[i].multiply(-1);
            }
        }
    }

    /**
     * Apply a controlled not gate (CNOT).
     * @param control The control qubit index
     * @param target The target qubit index
     */
    public void applyCNOT(int control, int target) { 
        validateQubitIndex(control);
        validateQubitIndex(target);

        int stateSize = 1 << this.numQubits;
        Complex[] newState = new Complex[stateSize];

        for (int i = 0; i < stateSize; i++) {
            newState[i] = new Complex(0, 0);
        }

        int controlMask = 1 << control;
        int targetMask  = 1 << target;

        for (int i = 0; i < stateSize; i++) {
            if ((i & controlMask) != 0) {
                newState[i ^ targetMask] = state[i];
            } else {
                newState[i] = state[i];
            }
        }
        state = newState;
    }

    /**
     * Measures a qubit and collapses the state to a 0 or 1
     * @param qubitIndex The index of the qubit to measure 
     * @return 0 or 1 
     */
    public int measure(int qubitIndex) {
        validateQubitIndex(qubitIndex);

        double probZero  = 0;
        int    mask      = 1 << qubitIndex;
        int    stateSize = 1 << this.numQubits;
        
        for (int i = 0; i < stateSize; i++) {
            if ((i & mask) == 0){
                probZero += state[i].magnitudeSquared();
            }
        }

        int result = random.nextDouble() < probZero ? 0 : 1;

        // Collapse the state now
        Complex[] newState = new Complex[stateSize];

        for (int i = 0; i < stateSize; i++) {
            newState[i] = new Complex(0,0);
        }

        if (result == 0){
            for (int i = 0; i < stateSize; i++){
                newState[i] = state[i].divide(Math.sqrt(probZero));
            }
        } else { // result equals 1 then
            double probOne = 1 - probZero; // small speedup
            for (int i = 0; i < stateSize; i++){
                newState[i] = state[i].divide(Math.sqrt(probOne));
            }
        }

        state = newState;
        return result;
    }

    /**
     * Get the current quantum state as a string for display.
     * @return String representation of the state.
     */
    public String getStateString(){
        StringBuilder stringBuilder = new StringBuilder();

        for (int i = 0; i < state.length; i ++){
            if (state[i].magnitude() > 1e-10) {
                @SuppressWarnings("UnnecessaryTemporaryOnConversionFromString")
                String binary = String.format("%0" + numQubits + "d", Integer.parseInt(Integer.toBinaryString(i)));
                stringBuilder.append(String.format("|%s>: %s(prob %.4f)\n", binary, state[i].toString(), state[i].magnitudeSquared()));
            }
        }

        return stringBuilder.toString();
    }

    /**
     * Get the raw quantum state
     * @return A copy of the exact quantum state
     */
    public Complex[] getState() {
        return Arrays.copyOf(state, state.length);
    }

    /**
     * Set the quantum state to an exact give state
     * @param newState The new state to set
     */
    public void setState(Complex[] newState){
        if(newState.length != this.state.length){
            throw new IllegalArgumentException("State size mismatch. Expected: " + this.state.length + " but got: " + newState.length);
        }
        this.state = Arrays.copyOf(newState, newState.length);
    }

    /**
     * Get the name of this quantum computer.
     * @return The name
     */
    public String getName() { 
        return name;
    }

    /**
     * Update the name of this quantum computer.
     * @param name The new name for this quantum computer
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * Get the number of qubits.
     * @return The number of qubits
     */
    public int getNumQubits(){
        return numQubits;
    }
    
    /**
     * Reset the quantum state to all 0s
     */
    public void reset(){
        this.initialize(this.numQubits);
    }

    @Override
    public String toString() {
        return "Quantum Computer{" + "name='" + name + "\'" + ", numQubits=" + numQubits + '}';
    }
    
}
