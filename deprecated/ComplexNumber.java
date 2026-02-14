// A complex number is a number that is a combination of a real and imaginary part
// An imaginary number is considered to be a number that is some real number times "i" the imaginary unit equal to sqrt(-1)
// As a convention of my code I will use the form (a + bi) for all complex numbers unless otherwise stated
public class ComplexNumber implements Comparable<ComplexNumber>{
    public final static double EPSILON = 0.00000001;
    public double a;
    public double b;

    public ComplexNumber(){
        this.a = 0;
        this.b = 0;
    }
    public ComplexNumber(double a, double b){
        this.a = a;
        this.b = b;
    }
    public ComplexNumber(double a){
        this.a = a;
        this.b = 0;
    }
    
    // multiplication and division
    public ComplexNumber mul(ComplexNumber c){
        return new ComplexNumber((this.a*c.a - this.b*c.b), (this.a*c.b + this.b*c.a));
    }
    public ComplexNumber div(ComplexNumber c){
        double realPart = (this.a*c.a + this.b*c.b)/(c.a*c.a + c.b*c.b);
        double imaginaryPart = (this.b*c.a - this.a*c.b)/(c.a*c.a + c.b*c.b);
        return new ComplexNumber(realPart, imaginaryPart); // this one is kinda complicated, so I split it up (there is no real reason to do this other than readability)
    }
    
    // addition and subtraction
    public ComplexNumber add(ComplexNumber c){
        return new ComplexNumber((this.a + c.a), (this.b + c.b));
    }
    public ComplexNumber sub(ComplexNumber c){
        return new ComplexNumber((this.a - c.a), (this.b - c.b));
    }

    // exponents and logarithms
    public ComplexNumber log(ComplexNumber c){
        double r1 = Math.log(Math.sqrt(c.a * c.a + c.b * c.b)); // these are the logs of the magnitudes it would be more technically correct to not call them "r" but I only ever use the log of the magnitudes
        double r2 = Math.log(Math.sqrt(this.a * this.a + this.b * this.b));
        double t1 = Math.atan2(c.b, c.a); // these are the theta values
        double t2 = Math.atan2(this.b, this.a);
        double realPart      = (r1*r2 + t1*t2)/(r2*r2 + t2*t2);
        double imaginaryPart = (r2*t1 - r1*t2)/(r2*r2 + t2*t2);
        return new ComplexNumber(realPart, imaginaryPart);
    }
    public ComplexNumber pow(ComplexNumber c){
        double r = Math.sqrt(this.a * this.a + this.b * this.b); // this time "r" is the proper magnitude
        double t = Math.atan2(this.b, this.a); // again this is theta
        double realPart      = Math.pow(r, c.a) / (Math.pow(Math.E, (c.b*t))) * Math.cos(c.b*Math.log(r) + c.a*t);
        double imaginaryPart = Math.pow(r, c.a) / (Math.pow(Math.E, (c.b*t))) * Math.sin(c.b*Math.log(r) + c.a*t);
        return new ComplexNumber(realPart, imaginaryPart);
    }

    // hyperbolic trig
    public ComplexNumber sinh(){
        ComplexNumber numerator = (new ComplexNumber(Math.E, 0)).pow(this).sub((new ComplexNumber(Math.E, 0)).pow(this.mul(new ComplexNumber(-1,0))));
        double realPart = numerator.a / 2;
        double imaginaryPart = numerator.b / 2;
        return new ComplexNumber(realPart, imaginaryPart);
    }

    public ComplexNumber cosh(){
        ComplexNumber numerator = (new ComplexNumber(Math.E, 0)).pow(this).add((new ComplexNumber(Math.E, 0)).pow(this.mul(new ComplexNumber(-1,0))));
        double realPart = numerator.a / 2;
        double imaginaryPart = numerator.b/2;
        return new ComplexNumber(realPart, imaginaryPart);
    }

    public ComplexNumber tanh(){
        // \frac{sinh}{cosh]
        ComplexNumber numerator = sinh();
        ComplexNumber denominator = cosh();
        return numerator.div(denominator);
    }

    public ComplexNumber coth(){
        // \frac{cosh}{sinh]
        ComplexNumber numerator = cosh();
        ComplexNumber denominator = sinh();
        return numerator.div(denominator);
    }

    public ComplexNumber sech(){
        // \frac{1}{cosh}
        ComplexNumber denominator = cosh();
        return new ComplexNumber(1, 0).div(denominator);
    }

    public ComplexNumber csch(){
        // \frac{1}{sinh}
        ComplexNumber denominator = sinh();
        return new ComplexNumber(1, 0).div(denominator);
    }

    // regular trig
    public ComplexNumber sin(){
        double bValue = this.b; // idk why I did this
        ComplexNumber  firstPart = (new ComplexNumber(Math.sin(a), 0)).mul((new ComplexNumber(bValue, 0).cosh()));
        ComplexNumber secondPart = (new ComplexNumber(Math.cos(a), 0)).mul((new ComplexNumber(bValue, 0).sinh()));
        return new ComplexNumber(firstPart.a, secondPart.a);
    }

    public ComplexNumber cos(){
        double bValue = this.b;
        ComplexNumber firstPart = (new ComplexNumber(Math.cos(a), 0)).mul((new ComplexNumber(bValue, 0).cosh()));
        ComplexNumber secondPart = (new ComplexNumber(Math.sin(a), 0)).mul((new ComplexNumber(bValue, 0).sinh()));
        return new ComplexNumber(firstPart.a, -1*secondPart.a);
    }

    public ComplexNumber tan(){
        ComplexNumber numerator = sin();
        ComplexNumber denominator = cos();
        return numerator.div(denominator);
    }

    public ComplexNumber cot(){
        ComplexNumber numerator = cos();
        ComplexNumber denominator = sin();
        return numerator.div(denominator);
    }

    public ComplexNumber sec(){
        ComplexNumber numerator = new ComplexNumber(1,0);
        ComplexNumber denominator = cos();
        return numerator.div(denominator);
    }

    public ComplexNumber csc(){
        ComplexNumber numerator = new ComplexNumber(1,0);
        ComplexNumber denominator = sin();
        return numerator.div(denominator);
    }

    // inverse hyperbolic trig
    public ComplexNumber asinh(){
        return new ComplexNumber(Math.E ,0).log(((((this.pow(new ComplexNumber(2,0))).add(new ComplexNumber(1,0))).pow(new ComplexNumber(0.5,0))).add(this)));
    }

    public ComplexNumber acosh(){

        return new ComplexNumber(Math.E ,0).log(((((this.pow(new ComplexNumber(2,0))).sub(new ComplexNumber(1,0))).pow(new ComplexNumber(0.5,0))).add(this)));
    }

    public ComplexNumber atanh(){

        return (new ComplexNumber(Math.E ,0).log( (this.add(new ComplexNumber(1,0))).div((new ComplexNumber(1,0).sub(this))))).mul(new ComplexNumber(0.5,0));
    }

    public ComplexNumber acoth(){
        return (new ComplexNumber(Math.E ,0).log( (this.add(new ComplexNumber(1,0))).div((this.sub(new ComplexNumber(1,0)))))).mul(new ComplexNumber(0.5,0));
    }

    public ComplexNumber asech(){
        return new ComplexNumber(Math.E, 0).log((new ComplexNumber(1,0).add((new ComplexNumber(1,0).sub(this.pow(new ComplexNumber(2,0)))).pow(new ComplexNumber(0.5,0)))).div(this));
    }

    public ComplexNumber acsch(){
        return new ComplexNumber(Math.E, 0).log((new ComplexNumber(1,0).add((new ComplexNumber(1,0).add(this.pow(new ComplexNumber(2,0)))).pow(new ComplexNumber(0.5,0)))).div(this));
    }

    // inverse regular trig
    public ComplexNumber asin(){
        return (new ComplexNumber(Math.E, 0).log((((new ComplexNumber(1,0).sub(this.pow(new ComplexNumber(2,0)))).pow(new ComplexNumber(0.5,0))).add(this.mul(new ComplexNumber(0,1)))))).mul(new ComplexNumber(0,-1));
    }

    public ComplexNumber acos(){
        return (new ComplexNumber(Math.E, 0).log((((new ComplexNumber(1,0).sub(this.pow(new ComplexNumber(2,0)))).pow(new ComplexNumber(0.5,0))).mul(new ComplexNumber(0,1)).add(this)))).mul(new ComplexNumber(0,-1));
    }

    public ComplexNumber atan(){
        return ((new ComplexNumber(Math.E, 0).log(((new ComplexNumber(0,1)).sub(this)).div(((new ComplexNumber(0,1)).add(this))))).mul(new ComplexNumber(0,-1))).div(new ComplexNumber(2,0));
    }

    public ComplexNumber acot(){
        return (new ComplexNumber(1,0).div(this)).atan();
    }

    public ComplexNumber asec(){
 
        return (new ComplexNumber(1,0).div(this)).acos();
    }

    public ComplexNumber acsc(){
        return (new ComplexNumber(1,0).div(this)).asin();
    }

    // factorials
    public Double gammaIntegral(double detail, ComplexNumber input, boolean real){
        // this is truly solving the Riemann sum of the gamma function over some small set
        double total = 0;
        if (real){
            for (double i = 0.000000000000000001; i < 30; i+=detail) {
                // this converges quickly (we will go to 30 for a good approximation (this gets us to the order of E-6 or E-7 precision)
                // I cant take log of zero, so I will get very close to zero to start with
                total += Math.pow(i,(input.a)) * (1/Math.pow(Math.E, (i))) * Math.cos(input.b * Math.log(i));
            }
        } else {
            // imaginary (only difference is the sin here where cos was used in the real part)
            for (double i = 0.000000000000000001; i < 30; i+=detail) {
                total += Math.pow(i,(input.a)) * (1/Math.pow(Math.E, (i))) * Math.sin(input.b * Math.log(i));
            }
        }
        total *= detail;
        return total;
    }
    public ComplexNumber gam(){
        // yes I dislike the abbreviation as well, but I want all of my functions to have a three or four letter abbreviation that is used
        // the gamma function is the extension of the factorial to all real numbers, what I am doing here is
        // analytically continuing this to a general complex number (not 0 or any negative integer which are out of the domain of the function)
        if((this.a == ((int)this.a) && this.a < 0)){
            return new ComplexNumber();
        } else if (this.a == 0 && this.b == 0) {
            return new ComplexNumber(1,0);
        } else if(this.a == 0){
            // when a == 0 the graph moves rapidly at the beginning so a smaller detail is required
            double realPart      = gammaIntegral(0.0005,this,true);
            // imaginary part = integral from 0 to infinity {t^{a-1}*e^{-t}*sin(b*ln{t})dt}
            double imaginaryPart = gammaIntegral(0.0005,this,false);
            return new ComplexNumber(realPart, imaginaryPart);
        }
        // real part = integral from 0 to infinity {t^{a-1}*e^{-t}*cos(b*ln{t})dt}
        // to evaluate the integral I will use a left Riemann sum
        double realPart      = gammaIntegral(0.2,this,true);
        // imaginary part = integral from 0 to infinity {t^{a-1}*e^{-t}*sin(b*ln{t})dt}
        double imaginaryPart = gammaIntegral(0.2,this,false);
        return new ComplexNumber(realPart, imaginaryPart);
    }
    // absolute value
    public ComplexNumber abs(){

        // the absolute value of a complex number is just the distance from the origin, use pythagoras to solve sqrt(z.a^2 + z.b^2) = |z|
        return new ComplexNumber(Math.sqrt(this.a*this.a + this.b*this.b), 0);
    }

    @Override
    public String toString() {
        if (this.a == 0){
            return this.b + "i";
        }
        if (this.b == 0){
            return this.a + "";
        }
        return this.a + " + " + this.b + "i";
    }

    @Override
    public int compareTo(ComplexNumber other) {
        return this.a > other.a ? 1 : -1;
    }

    @Override
    public int hashCode() {
        return (int) (this.a * 1000000 + this.b * 1000000);
    }

    @Override
    public boolean equals(Object obj) {
        if (!(obj instanceof ComplexNumber)){
            return false;
        }
        if(((ComplexNumber) obj).a != this.a){
            return Math.abs(((((ComplexNumber) obj).a)) - (this.a)) <= EPSILON;
        }
        if(((ComplexNumber) obj).b != this.b){
            return Math.abs(((((ComplexNumber) obj).b)) - (this.b)) <= EPSILON;
        }
        return true;
    }
} 
