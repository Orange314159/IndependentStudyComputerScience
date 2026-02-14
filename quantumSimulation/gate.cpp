#include "gate.h"

Gate::Gate(const Matrix& matrix, int numQubits, const std::string& name) {
    this->gateMatrix = matrix;
    this->numQubits = numQubits;
    this->name = name;
}

Gate::Gate() {
    this->gateMatrix = Matrix();
    this->numQubits = 0;
    this->name = "";
}

Gate::Gate(const Matrix& matrix) {
    this->gateMatrix = matrix;
    this->numQubits = matrix.rows;
    this->name = "";
}

Gate Gate::combineGates(Gate& other){
    return *(new Gate(this->gateMatrix.tensorProduct(other.gateMatrix)));
}