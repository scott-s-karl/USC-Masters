// Steven Karl
// 6-20-2021

//Packages
import java.lang.Math;


//Class
class ComplexNumber {
        // Public Members
        double real;
        double imag;

        // Constructor
        ComplexNumber(double i, double j){
                real = i;
                imag = j;
        }
        // Methods
        public ComplexNumber add(ComplexNumber c2){
                real = real + c2.real;
                imag = imag + c2.imag;
                ComplexNumber c = new ComplexNumber(real, imag);
                return(c);
        }
        public ComplexNumber squared(){
                double tmpReal = real;
                real = (real * real) - (imag * imag);
                imag = (tmpReal * imag) + (imag * tmpReal);
                ComplexNumber c = new ComplexNumber(real, imag);
                return(c);
        }
        public double abs(){
                return(Math.sqrt(Math.pow(real,2) + Math.pow(imag,2)));
        }
}
