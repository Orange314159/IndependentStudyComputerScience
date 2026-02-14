public class Qubit extends Matrix{

    public Qubit(){
        this.cols = 0;
        this.rows = 0;
        this.data = new ComplexNumber[0][0];
    }
    
    public Qubit(int rows){
        this.rows = rows;
        this.cols = 1;
        this.data = new ComplexNumber[rows][1];
    }

    public Qubit(int rows, ComplexNumber[][] data){
        this.rows = rows;
        this.cols = 1;
        this.data = data;
    }

}