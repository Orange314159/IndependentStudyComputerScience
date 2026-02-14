package com.core;


/**
 * This class represents a complex number for quantum state amplitudes.
 */
public class Complex {
    private double real;
    private double imaginary;
    private  static final double EPSILON = 1e-15;

    
    /**
     * Create a complex number.
     * @param real The real part
     * @param imaginary The imaginary part
     */
    public Complex(double real, double imaginary){
        this.real = real;
        this.imaginary = imaginary;
    }

    /**
     * Create a complex number with only real parts.
     * @param real
     */
    public Complex(double real){
        this.real = real;
        this.imaginary = 0;
    }

    /**
     * Add another complex number to this complex number.
     * @param other The other complex number
     * @return The sum of the two complex numbers
     */
    public Complex add(Complex other){
        return new Complex(this.real + other.real, this.imaginary + other.imaginary);
    }

    /**
     * Subtract another complex number from this number.
     * @param other The other complex number
     * @return The difference between the two complex numbers: this - other
     */
    public Complex subtract(Complex other){
        return new Complex(this.real - other.real, this.imaginary-other.imaginary);
    }

    /**
     * Multiply by another complex number.
     * @param other The other complex number
     * @return The product of the two complex numbers
     */
    public Complex multiply(Complex other){
        double newReal = this.real * other.real - this.imaginary * other.imaginary;
        double newImaginary = this.real * other.imaginary + this.imaginary * other.real;
        return new Complex(newReal, newImaginary);
    }

    /**
     * Multiply by a scalar value.
     * @param other The scalar value
     * @return The product of this number and the scalar
     */
    public Complex multiply(double scalar){
        return new Complex(this.real * scalar, this.imaginary * scalar);
    }

    /**
     * Divide by a complex number
     * @param other The complex number to divide this by.
     * @return The quotient
     */
    public Complex divide(Complex other){
        double realPart = (this.real*other.real + this.imaginary*other.imaginary)/(other.real*other.real + other.imaginary*other.imaginary);
        double imaginaryPart = (this.imaginary*other.real - this.real*other.imaginary)/(other.real*other.real + other.imaginary*other.imaginary);
        return new Complex(realPart, imaginaryPart);
    }
    
    /**
     * Divide by a scalar
     * @param other The other scalar value.
     * @return The quotient
     */
    public Complex divide(double scalar){
        if (Math.abs(scalar) < EPSILON){
            throw new ArithmeticException("Division by zero");
        }
        return new Complex(this.real / scalar, this.imaginary / scalar);
    }

    /**
     * Returns the complex conjugate of this complex number.
     * @return The conjugate
     */
    public Complex conjugate(){
        return new Complex(this.real, -this.imaginary);
    }

    /**
     * Returns the magnitude of the complex number using Pythagoras.
     * @return The magnitude
     */
    public double magnitude(){
        return Math.sqrt(this.real * this.real + this.imaginary * this.imaginary);
    }

    /**
     * Returns the magnitude by squared.
     * @return The magnitude squared
     */
    public double magnitudeSquared(){
        return this.real * this.real + this.imaginary * this.imaginary;
    }

    /**
     * Get the real part.
     * @return The real part
     */
    public double real(){
        return this.real;
    }

    /**
     * Get the imaginary part.
     * @return The imaginary part
     */
    public double imaginary(){
        return this.imaginary;
    }

    /**
     * Returns a string of the complex number rounded to six places
     * @return A string representation of the complex number
     */
    @Override
    public String toString(){
        if (Math.abs(this.imaginary) < EPSILON){
            return String.format("%.6f", this.real);
        } else if (Math.abs(this.real) < EPSILON){
            return String.format("%.6fi", this.imaginary);
        } else {
            String sign = this.imaginary >=0 ? "+" : "";
            return String.format("%.6f%s%.6fi", this.real, sign, this.imaginary);
        }
    }

    /**
     * Does a simple comparison between the real and imaginary parts.
     * @return If the two complex numbers are equal
     */
    @Override
    public boolean equals(Object obj){
        if (! (obj instanceof Complex other)){
            return false;
        }
        return Math.abs(this.real - other.real) < EPSILON && Math.abs(this.imaginary - other.imaginary) < EPSILON;
    }

    /**
     * Generates hash code
     * @return The hash code
     */
    @Override
    public int hashCode(){
        return Double.hashCode(real) ^ Double.hashCode(imaginary);
    }

}
