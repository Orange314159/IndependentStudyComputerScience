#ifndef QUBIT_H
#define QUBIT_H

#include "matrix.h"
#include "gate.h"
#include <string>
#include <vector>

class Qubit {
public:
    // sets of qubits are generally represented as a vector
    Matrix data;
    int numQubits;
    std::string name;

    // Constructor
    Qubit(const Matrix& matrix, int numQubits, const std::string& name);

    Qubit();

    Qubit(const Matrix& matrix);


    Qubit applyGate(Gate& gate);    
};

#endif // QUBIT_H