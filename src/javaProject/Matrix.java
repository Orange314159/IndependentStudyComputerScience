public class Matrix {
    public int rows = 0;
    public int cols = 0;
    public ComplexNumber[][] data;

    public Matrix(){
        this.rows = 0;
        this.cols = 0;
        this.data = new ComplexNumber[0][0];
    }

    public Matrix(int rows, int cols){
        this.rows = rows;
        this.cols = cols;
        this.data = new ComplexNumber[(int)rows][(int)cols];
    }

    public Matrix(int rows, int cols, ComplexNumber[][] data){
        this.rows = rows;
        this.cols = cols;
        this.data = data;
    }

    public Matrix(ComplexNumber[][] data){
        this.rows = data.length;
        this.cols = data[0].length;
        this.data = data;
    }

    void setIdentity(int rows){
        // Creates an identity matrix of 'rows' rows
        this.rows = rows;
        this.cols = rows;
        data = new ComplexNumber[(int)rows][(int)rows];
        // fill all diagonal entries with a 1
        for (int i = 0; i < rows; i++){
            data[(int)i][(int)i] = new ComplexNumber(1.0);
        }
    }

    void setIdentity(){
        // in the case that we don't want to change the size of the matrix while making the identity
        if (rows != cols){
            System.out.println("Error: can't set a non square matrix to identity.");
            return;
        }
        this.setIdentity(this.rows);
    }

    Matrix inverse(){
        if (rows != cols) {
            System.out.println("Error: can only find the inverse of a square matrix.");
            return null;
        }

        int n = (int)rows;
        
        ComplexNumber[][] augmentedData = new ComplexNumber[n][2 * n];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                augmentedData[i][j] = this.data[i][j];
            }
            augmentedData[i][i + n] = new ComplexNumber(1.0);
        }

        for (int i = 0; i < n; i++) { 
            
            int pivotRow = i;
            for (int k = i + 1; k < n; k++) {
                if ((augmentedData[k][i]).abs().a > (augmentedData[pivotRow][i]).abs().a) {
                    pivotRow = k;
                }
            }
            
            if (pivotRow != i) {
                ComplexNumber[] temp = augmentedData[i];
                augmentedData[i] = augmentedData[pivotRow];
                augmentedData[pivotRow] = temp;
            }

            ComplexNumber pivot = augmentedData[i][i];
            if ((pivot).abs().a < 1e-10 && (pivot).abs().b < 1e-10) { 
                System.out.println("Error: Matrix is singular and not invertible.");
                return null;
            }

            for (int j = i; j < 2 * n; j++) {
                augmentedData[i][j] = augmentedData[i][j].div(pivot);
            }

            for (int k = 0; k < n; k++) {
                if (k != i) {
                    ComplexNumber factor = augmentedData[k][i];
                    for (int j = i; j < 2 * n; j++) {
                        augmentedData[k][j] = augmentedData[k][j].sub(factor.mul(augmentedData[i][j]));
                    }
                }
            }
        }

        ComplexNumber[][] inverseData = new ComplexNumber[n][n];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                inverseData[i][j] = augmentedData[i][j + n];
            }
        }

        return new Matrix(rows, cols, inverseData);
    }

    Matrix tensorProduct(Matrix other){
        Matrix outputMatrix = new Matrix(this.rows * other.rows, this.cols * other.cols);

        for (int row = 0; row < outputMatrix.rows; row++){
            for (int col = 0; col < outputMatrix.cols; col++){
                outputMatrix.data[row][col] = this.data[(int)(row/other.rows)][(int)(col/other.cols)].mul(other.data[(int)(row%other.rows)][(int)(col%other.cols)]);
            }
        }

        return outputMatrix;
    }

    Matrix addMatrix(Matrix other){
        if (rows == 0 || cols == 0 || other.rows == 0 || other.cols == 0) {
            throw new IllegalArgumentException("Adding a zero dimension matrix is not allowed.");
        }
        // Two matrices can only be added if they have the same dimensions
        if (rows != other.rows || cols != other.cols) {
            throw new IllegalArgumentException("Matrices must have the same dimensions for addition.");
        }
        
        // New matrix used to hold the result
        Matrix result = new Matrix(rows, cols);
        
        // Matrix addition is performed by adding corresponding elements
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                result.data[i][j] = this.data[i][j].add(other.data[i][j]);
            }
        }
        return result;
    }

    Matrix subMatrix(Matrix other){
        if (rows == 0 || cols == 0 || other.rows == 0 || other.cols == 0) {
            throw new IllegalArgumentException("Subtracting a zero dimension matrix is not allowed.");
        }
        // Two matrices can only be subtracted if they have the same dimensions
        if (rows != other.rows || cols != other.cols) {
            throw new IllegalArgumentException("Matrices must have the same dimensions for subtraction.");
        }
        
        // New matrix used to hold the result
        Matrix result = new Matrix(rows, cols);
        
        // Matrix subtraction is performed by subtracting corresponding elements
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                result.data[i][j] = this.data[i][j].sub(other.data[i][j]);
            }
        }
        return result;

    }

    Matrix multMatrix(Matrix other){
        if (this.cols != other.rows) {
            throw new IllegalArgumentException("Number of columns of first matrix must equal number of rows of second matrix for multiplication.");
        }
        
        // The resulting matrix has dimensions (this.rows x other.cols)
        Matrix result = new Matrix(this.rows, other.cols);
        
        // Matrix multiplication (O(n^3) implementation)
        for (int i = 0; i < this.rows; i++) { // Row of the resulting matrix
            for (int j = 0; j < other.cols; j++) { // Column of the resulting matrix
                ComplexNumber sum = new ComplexNumber(0.0);
                for (int k = 0; k < this.cols; k++) { // Element index for dot product
                    sum = sum.add(this.data[i][k].mul(other.data[k][j]));
                }
                result.data[i][j] = sum;
            }
        }
        return result;

    }

    Matrix divMatrix(Matrix other){
        Matrix otherInverse = other.inverse();
        return this.multMatrix(otherInverse);
    }

    public String toString(){
        StringBuilder stringBuilder = new StringBuilder();

        for (int i = 0; i < (int)this.rows; i ++){
            for (int j = 0; j < (int)this.cols; j++){
                stringBuilder.append(data[i][j]).append('\t'); 
            }
            stringBuilder.append('\n');
        }
        return stringBuilder.toString();
    }

}