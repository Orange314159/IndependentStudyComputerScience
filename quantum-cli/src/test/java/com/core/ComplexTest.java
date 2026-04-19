package com.core;

import org.junit.Test;
import static org.junit.Assert.*;

public class ComplexTest {

    @Test
    public void testConstructorWithRealAndImaginary() {
        Complex c = new Complex(1.0, 2.0);
        assertEquals(1.0, c.real(), 0.0);
        assertEquals(2.0, c.imaginary(), 0.0);
    }

    @Test
    public void testConstructorWithRealOnly() {
        Complex c = new Complex(3.0);
        assertEquals(3.0, c.real(), 0.0);
        assertEquals(0.0, c.imaginary(), 0.0);
    }

    @Test
    public void testAdd() {
        Complex c1 = new Complex(1.0, 2.0);
        Complex c2 = new Complex(3.0, 4.0);
        Complex result = c1.add(c2);
        assertEquals(4.0, result.real(), 0.0);
        assertEquals(6.0, result.imaginary(), 0.0);
    }

    @Test
    public void testSubtract() {
        Complex c1 = new Complex(5.0, 3.0);
        Complex c2 = new Complex(2.0, 1.0);
        Complex result = c1.subtract(c2);
        assertEquals(3.0, result.real(), 0.0);
        assertEquals(2.0, result.imaginary(), 0.0);
    }

    @Test
    public void testMultiplyComplex() {
        Complex c1 = new Complex(1.0, 2.0);
        Complex c2 = new Complex(3.0, 4.0);
        Complex result = c1.multiply(c2);
        assertEquals(-5.0, result.real(), 0.0);
        assertEquals(10.0, result.imaginary(), 0.0);
    }

    @Test
    public void testMultiplyScalar() {
        Complex c = new Complex(2.0, 3.0);
        Complex result = c.multiply(2.0);
        assertEquals(4.0, result.real(), 0.0);
        assertEquals(6.0, result.imaginary(), 0.0);
    }

    @Test
    public void testDivideComplex() {
        Complex c1 = new Complex(1.0, 2.0);
        Complex c2 = new Complex(1.0, 1.0);
        Complex result = c1.divide(c2);
        assertEquals(1.5, result.real(), 0.001);
        assertEquals(0.5, result.imaginary(), 0.001);
    }

    @Test
    public void testDivideScalar() {
        Complex c = new Complex(4.0, 6.0);
        Complex result = c.divide(2.0);
        assertEquals(2.0, result.real(), 0.0);
        assertEquals(3.0, result.imaginary(), 0.0);
    }

    @Test(expected = ArithmeticException.class)
    public void testDivideByZeroScalar() {
        Complex c = new Complex(1.0, 1.0);
        c.divide(0.0);
    }

    @Test
    public void testConjugate() {
        Complex c = new Complex(1.0, 2.0);
        Complex result = c.conjugate();
        assertEquals(1.0, result.real(), 0.0);
        assertEquals(-2.0, result.imaginary(), 0.0);
    }

    @Test
    public void testMagnitude() {
        Complex c = new Complex(3.0, 4.0);
        assertEquals(5.0, c.magnitude(), 0.0);
    }

    @Test
    public void testMagnitudeSquared() {
        Complex c = new Complex(3.0, 4.0);
        assertEquals(25.0, c.magnitudeSquared(), 0.0);
    }

    @Test
    public void testToString() {
        Complex c1 = new Complex(1.5, 0.0);
        assertEquals("1.500000", c1.toString());
        Complex c2 = new Complex(0.0, 2.5);
        assertEquals("2.500000i", c2.toString());
        Complex c3 = new Complex(1.0, -2.0);
        assertEquals("1.000000-2.000000i", c3.toString());
    }

    @Test
    public void testEquals() {
        Complex c1 = new Complex(1.0, 2.0);
        Complex c2 = new Complex(1.0, 2.0);
        Complex c3 = new Complex(1.0 + 1e-16, 2.0); // within epsilon
        assertTrue(c1.equals(c2));
        assertTrue(c1.equals(c3));
        assertFalse(c1.equals(new Complex(1.1, 2.0)));
        assertFalse(c1.equals("not complex"));
    }

    @Test
    public void testHashCode() {
        Complex c1 = new Complex(1.0, 2.0);
        Complex c2 = new Complex(1.0, 2.0);
        assertEquals(c1.hashCode(), c2.hashCode());
    }
}