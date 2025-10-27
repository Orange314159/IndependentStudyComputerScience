#ifndef MATRIX_H
#define MATRIX_H

#include <stdexcept>
#include <string>
#include <cmath>

class Matrix {
public:
    int rows;
    int cols;
    double** data;

    Matrix(int rows, int cols);
    Matrix(); // Default constructor
    ~Matrix(); // Destructor

    void setIdentity();
    Matrix inverse();

    // Operator overloads
    Matrix operator+(const Matrix& other);
    Matrix operator-(const Matrix& other);
    Matrix operator*(const Matrix& other);
    Matrix operator/(const Matrix& other); // Declaration for division

    std::string toString();
};

#endif // MATRIX_H
