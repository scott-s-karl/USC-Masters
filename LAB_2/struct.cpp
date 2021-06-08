// Steven Karl

#include <iostream>

struct complex{
        double m;
        double n;
        void add(complex b){
                m = m + b.m;
                n = n + b.n;
        };
        void multiply(complex b){
                double tmp = m;
                m = (m * b.m) - (n * b.n);
                n = (tmp * b.n) + (n * b.m);
        };
        void conjugate(){
                n = -n;
        };
        void print(){
                printf("The number is: %f %fi\n",m,n);
        };
};

int main(int argc, char const *argv[]) {
        complex x={3,4};
        complex y={1,-0.25};
        x.print();
        y.print();

        x.add(y);
        x.print();

        y.multiply(x);
        y.print();

        x.conjugate();
        x.print();

        y.conjugate();
        y.print();

        return 0;
}
