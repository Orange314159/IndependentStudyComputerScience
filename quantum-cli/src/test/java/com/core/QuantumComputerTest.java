package com.core;

import org.junit.Test;
import static org.junit.Assert.*;

public class QuantumComputerTest {

    @Test
    public void testConstructor() {
        QuantumComputer qc = new QuantumComputer("test", 2);
        assertEquals("test", qc.getName());
        assertEquals(2, qc.getNumQubits());
        Complex[] state = qc.getState();
        assertEquals(4, state.length);
        assertEquals(1.0, state[0].real(), 0.0);
        assertEquals(0.0, state[0].imaginary(), 0.0);
        for (int i = 1; i < 4; i++) {
            assertEquals(0.0, state[i].real(), 0.0);
            assertEquals(0.0, state[i].imaginary(), 0.0);
        }
    }

    @Test
    public void testSetName() {
        QuantumComputer qc = new QuantumComputer("test", 1);
        qc.setName("newName");
        assertEquals("newName", qc.getName());
    }

    @Test
    public void testReset() {
        QuantumComputer qc = new QuantumComputer("test", 1);
        qc.applyHadamard(0); // change state
        qc.reset();
        Complex[] state = qc.getState();
        assertEquals(1.0, state[0].real(), 0.0);
        assertEquals(0.0, state[1].real(), 0.0);
    }

    @Test
    public void testApplyHadamard() {
        QuantumComputer qc = new QuantumComputer("test", 1);
        qc.applyHadamard(0);
        Complex[] state = qc.getState();
        double expected = 1.0 / Math.sqrt(2);
        assertEquals(expected, state[0].real(), 1e-10);
        assertEquals(expected, state[1].real(), 1e-10);
    }

    @Test
    public void testApplyPauliX() {
        QuantumComputer qc = new QuantumComputer("test", 1);
        qc.applyPauliX(0);
        Complex[] state = qc.getState();
        assertEquals(0.0, state[0].real(), 0.0);
        assertEquals(1.0, state[1].real(), 0.0);
    }

    @Test
    public void testApplyPauliZ() {
        QuantumComputer qc = new QuantumComputer("test", 1);
        qc.applyHadamard(0); // |+>
        qc.applyPauliZ(0); // should become |->
        Complex[] state = qc.getState();
        double expected = 1.0 / Math.sqrt(2);
        assertEquals(expected, state[0].real(), 1e-10);
        assertEquals(-expected, state[1].real(), 1e-10);
    }

    @Test
    public void testApplyCNOT() {
        QuantumComputer qc = new QuantumComputer("test", 2);
        qc.applyHadamard(0); // control in superposition
        qc.applyCNOT(0, 1);
        Complex[] state = qc.getState();
        double expected = 1.0 / Math.sqrt(2);
        assertEquals(expected, state[0].real(), 1e-10); // |00>
        assertEquals(0.0, state[1].real(), 1e-10); // |01>
        assertEquals(0.0, state[2].real(), 1e-10); // |10>
        assertEquals(expected, state[3].real(), 1e-10); // |11>
    }

    @Test
    public void testSetState() {
        QuantumComputer qc = new QuantumComputer("test", 1);
        Complex[] newState = {new Complex(0, 0), new Complex(1, 0)};
        qc.setState(newState);
        Complex[] state = qc.getState();
        assertEquals(0.0, state[0].real(), 0.0);
        assertEquals(1.0, state[1].real(), 0.0);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSetStateWrongSize() {
        QuantumComputer qc = new QuantumComputer("test", 1);
        Complex[] newState = {new Complex(1, 0)}; // wrong size
        qc.setState(newState);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testInvalidQubitIndex() {
        QuantumComputer qc = new QuantumComputer("test", 1);
        qc.applyHadamard(1); // out of bounds
    }

    @Test
    public void testToString() {
        QuantumComputer qc = new QuantumComputer("test", 1);
        assertEquals("Quantum Computer{name='test', numQubits=1}", qc.toString());
    }
}