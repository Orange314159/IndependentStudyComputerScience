public class Gate extends Matrix {
    

    public void setIdentity(){
        super.setIdentity();
    }

    public void setHadamard(){
        ComplexNumber invSqrtTwo = new ComplexNumber(1.0 / Math.sqrt(2.0));
        this.rows = 2;
        this.cols = 2;
        this.data = new ComplexNumber[][] {
            {invSqrtTwo, invSqrtTwo},
            {invSqrtTwo, invSqrtTwo.mul(new ComplexNumber(-1))}
        };
    }

    public void setX(){

    }
}