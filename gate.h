#ifndef GATE_H
#define GATE_H

#include "matrix.h"
#include <string>
#include <vector>

class Gate {
public:
    // The matrix representation of the quantum gate
    Matrix gateMatrix;
    // The number of qubits this gate acts upon
    int numQubits;
    // a name for the gate
    std::string name;

    // Constructor
    Gate(const Matrix& matrix, int numQubits, const std::string& name);

    Gate();
    
    Gate(const Matrix& matrix);

    // method to combine multiple gates (this is just a tensor product really)
    Gate combineGates(Gate& other);
    
};

#endif // GATE_H