// Steven Karl

#include <iostream>
#include <math.h>

double mean(double a, double b, int meanType = 0){

        //Local variables
        double mean;

        // Figure out the Type
        switch(meanType){
                case 0:
                        printf("Arithmetic\n");
                        printf("----------\n");
                        mean = (a + b)/2;

                        break;
                case 1:
                        printf("Geometric\n");
                        printf("---------\n");
                        mean = sqrt(a + b);
                        break;
                case 2:
                        printf("Harmonic\n");
                        printf("--------\n");
                        mean = 1/((1/a) + (1/b));
                        break;
        }
        return(mean);
}
int main(int argc, char const *argv[]) {
        /* code */
        printf("%f\n\n", mean(3,5,0));
        printf("%f\n\n", mean(3,5,1));
        printf("%f\n\n", mean(3,5,2));
        printf("%f\n\n", mean(3,5));
        return 0;
}
