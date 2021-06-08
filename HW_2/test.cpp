#include <iostream>


int main(int argc, char const *argv[]) {

        int xRes=24, yRes=12;

        float **xtable; // pointer to pointer[s]
        xtable = new float*[yRes]; // a ("column") array to hold 'float *' type pointers

        // each element of the above array points to a 'row' of xRes elements
        for(int i=0;i < yRes;i++) {
                xtable[i] = new float[xRes];
        }

        for(int i=0;i < yRes;i++){
                for(int j=0;j < xRes;j++){
                        xtable[i][j] = 10; // store 45 for pixel data, "for now"
                }
        }
        for(int i=0;i < yRes;i++){
                for(int j=0;j < xRes;j++){
                        printf("%.0f ", xtable[0][0]); // store 45 for pixel data, "for now"
                }
                printf("\n");
        }
        return 0;
}
