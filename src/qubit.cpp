#include "qubit.h"

Qubit::Qubit(const Matrix& matrix, int numQubits, const std::string& name) {
    this->data = matrix;
    this->numQubits = numQubits;
    this->name = name;
}

Qubit::Qubit() {
    this->data = Matrix();
    this->numQubits = 0;
    this->name = "";
}

Qubit::Qubit(const Matrix& matrix) {
    this->data = matrix;
    this->numQubits = matrix.rows;
    this->name = "";
}


Qubit Qubit::applyGate(Gate& gate){
    return *(new Qubit(this->data * gate.gateMatrix));
}