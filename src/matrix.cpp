// #include <iostream>
#include <stdexcept>
#include <string>
#include <cmath>


class Matrix {
    public:
        // initialize a row and col for the matrix
        int rows;
        int cols;
        // intialize a 2d array to hold all of the matrix elements
        int** data;

        // constructor to create a matrix of given rows and columns
        Matrix(int rows, int cols){
            this->rows = rows;
            this->cols = cols;
            data = new int*[rows];
            for(int i = 0; i < rows; i++) {
                data[i] = new int[cols]();
            }
        }

        // allow for the easy creation of identity matrices
        void setIdentity() {
            if (rows != cols) {
                throw std::invalid_argument("Only square matrices can be identity matrices. (attempted to set a " + std::to_string(rows) + " by " + std::to_string(cols) + " matrix to identity).");
            }
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    if (i == j) {
                        data[i][j] = 1;
                    } else {
                        data[i][j] = 0;
                    }
                }
            }
        }

        // default constructor
        Matrix() {
            rows = 0;
            cols = 0;
            data = nullptr;
        }

        // define matrix inversion for any square matrix using Gauss-Jordan elimination
        Matrix inverse() {
            if (rows != cols) {
                throw std::invalid_argument("Only square matrices can be inverted. (attempted to invert a " + std::to_string(rows) + " by " + std::to_string(cols) + " matrix).");
            }
            // to find the inverse of a matrix it is best to first augment it with the identity matrix
            Matrix augmented(rows, cols * 2);
            // fill in the augmented matrix
            for (int i = 0; i < rows; i++) {
                // adding in the original matrix data
                for (int j = 0; j < cols; j++) {
                    augmented.data[i][j] = data[i][j];
                }
                // adding in the identity matrix data
                for (int j = cols; j < cols * 2; j++) {
                    if (j - cols == i) {
                        augmented.data[i][j] = 1;
                    } else {
                        augmented.data[i][j] = 0;
                    }
                }
            }

            // now preforming Gauss-Jordan elimination
            int maxRow = 0;
            for (int i = 0; i < cols; i++) {
                // to perforom elimination we want to diagonalize the 1s in the matrix, this will involve going through each col and attempting to make one of the rows in it a 1
                
                // look for the row with the greatest magnitude, we will then swap this row with the top 
                for (int j = i; j < rows; j++) {
                    
                    if (std::abs(augmented.data[j][i]) > std::abs(augmented.data[maxRow][i])){
                        maxRow = j;
                    }

                }
                
            }
        }

        // opperation overloading is pretty great
        Matrix operator+(const Matrix& other) {
            if (rows == 0 || cols == 0 || other.rows == 0 || other.cols == 0) {
                throw std::invalid_argument("Adding a zero dimension matrix is not allowed.");
            }
            // two matricies can only be added if they have the same dimensions
            if (rows != other.rows || cols != other.cols) {
                throw std::invalid_argument("Matrices must have the same dimensions for addition.");
            }
            // new matrix used to hold the reuslt of this opperations
            Matrix result(rows, cols);
            // matrix addition in performed by adding corresponding elements
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    result.data[i][j] = data[i][j] + other.data[i][j];
                }
            }
            return result;
        }
        
        // subtraction follow the same process as addition, but just subracting corresponding elements instead of adding
        Matrix operator-(const Matrix& other) {
            if (rows == 0 || cols == 0 || other.rows == 0 || other.cols == 0) {
                throw std::invalid_argument("Subtracting a zero dimension matrix is not allowed.");
            }
            // two matricies can only be subtracted if they have the same dimensions
            if (rows != other.rows || cols != other.cols) {
                throw std::invalid_argument("Matrices must have the same dimensions for subtraction.");
            }
            // new matrix used to hold the reuslt of this opperations
            Matrix result(rows, cols);
            // matrix subtraction in performed by subtracting corresponding elements
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    result.data[i][j] = data[i][j] - other.data[i][j];
                }
            }
            return result;
        }

        // multiplication is a bit more complex
        Matrix operator*(const Matrix& other) {
            // two matricies can only be multiplied if the number of columns in the first matrix equals the number of rows in the second matrix
            if (cols != other.rows) {
                throw std::invalid_argument("Number of columns of first matrix must equal number of rows of second matrix for multiplication.");
            }
            // new matrix used to hold the reuslt of this opperations
            Matrix result(rows, other.cols);
            // matrix multiplication is performed by taking the dot product of rows and columns (gotta love some good O(n^3))
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < other.cols; j++) {
                    for (int k = 0; k < cols; k++) {
                        result.data[i][j] += data[i][k] * other.data[k][j];
                    }
                }
            }
            // i might consider using the Schwartz, Vaknin method in the future to speed up this multiplication but for now this will do (10,21,2025)
            return result;
        }

        // matrix division is not technically defined, but we can define it as multiplying by the inverse of the other matrix
        Matrix operator/(const Matrix& other) {
            
        }

        // to string method for easy print and debug 
        std::string toString() {
            std::string result = "";
            // loop through each element and print it out 
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    result += std::to_string(data[i][j]) + " ";
                }
                // remember to include a new line after each row
                result += "\n";
            }
            // the final string has one too many new lines, so ill remove the last character \n before returning
            result.pop_back();
            return result;
        }



        // good practice to free allocated memory in destructor
        ~Matrix() {
            for(int i = 0; i < rows; i++) {
                delete[] data[i];
            }
            delete[] data;
        }
};