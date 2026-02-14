#include <iostream>
#include "matrix.h"

int main() {
    // Test default constructor
    Matrix m_default;
    std::cout << "Default Matrix (should be 0x0):" << std::endl;
    std::cout << "Rows: " << m_default.rows << ", Cols: " << m_default.cols << std::endl;
    std::cout << "--------------------" << std::endl;

    // Test constructor with dimensions
    Matrix m1(2, 2);
    std::cout << "Matrix m1 (2x2, initialized to zeros):" << std::endl;
    std::cout << m1.toString();
    std::cout << "--------------------" << std::endl;

    // Test setIdentity
    Matrix m_identity(3, 3);
    m_identity.setIdentity();
    std::cout << "Identity Matrix (3x3):" << std::endl;
    std::cout << m_identity.toString();
    std::cout << "--------------------" << std::endl;

    // Test addition
    Matrix a(2, 2);
    a.data[0][0] = 1; a.data[0][1] = 2;
    a.data[1][0] = 3; a.data[1][1] = 4;

    Matrix b(2, 2);
    b.data[0][0] = 5; b.data[0][1] = 6;
    b.data[1][0] = 7; b.data[1][1] = 8;

    std::cout << "Matrix A:" << std::endl;
    std::cout << a.toString();
    std::cout << "Matrix B:" << std::endl;
    std::cout << b.toString();

    Matrix sum = a + b;
    std::cout << "A + B:" << std::endl;
    std::cout << sum.toString();
    std::cout << "--------------------" << std::endl;

    // Test subtraction
    Matrix diff = b;

    // Test inverse
    Matrix c(2, 2);
    c.data[0][0] = 1; c.data[0][1] = 2;
    c.data[1][0] = 3; c.data[1][1] = 4;

    std::cout << "Matrix C:" << std::endl;
    std::cout << c.toString();

    Matrix c_inverse = c.inverse();
    std::cout << "Inverse of Matrix C:" << std::endl;
    std::cout << c_inverse.toString();
    std::cout << "--------------------" << std::endl;

    // Test inverse of a 5x5 matrix
    Matrix to_invert(5, 5);
    to_invert.data[0][0] = 1; to_invert.data[0][1] = 2; to_invert.data[0][2] = 3; to_invert.data[0][3] = 4; to_invert.data[0][4] = 5;
    to_invert.data[1][0] = -6; to_invert.data[1][1] = -7; to_invert.data[1][2] = -8; to_invert.data[1][3] = -9; to_invert.data[1][4] = -10;
    to_invert.data[2][0] = 10; to_invert.data[2][1] = 11; to_invert.data[2][2] = 11; to_invert.data[2][3] = 11; to_invert.data[2][4] = 11;
    to_invert.data[3][0] = 678; to_invert.data[3][1] = 9876; to_invert.data[3][2] = 9786; to_invert.data[3][3] = 56; to_invert.data[3][4] = 45;
    to_invert.data[4][0] = 468; to_invert.data[4][1] = 5687; to_invert.data[4][2] = 87; to_invert.data[4][3] = 5; to_invert.data[4][4] = 85;

    std::cout << "Matrix to Invert:" << std::endl;
    std::cout << to_invert.toString();

    Matrix inverted_matrix = to_invert.inverse();
    std::cout << "Inverted Matrix:" << std::endl;
    std::cout << inverted_matrix.toString();
    std::cout << "--------------------" << std::endl;
}