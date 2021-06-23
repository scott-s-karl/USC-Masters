// Steven Karl


//Libraries
#include <iostream>
#include "Q3EC2.h"
#include <math.h>
#include <cstring>

// array2D class definitions
array2D::array2D(int xResolution, int yResolution){
        xRes = xResolution;
        yRes = yResolution;
        //xtable = new float*[yRes];
        xtable = (float **)calloc(yRes, sizeof(*xtable));
        for(int i = 0; i < yRes; i++){
                xtable[i] = (float *)calloc(xRes, sizeof(float));
                //xtable[i] = new float[xRes];
        }
}
array2D::~array2D(){
        for(int i = 0; i < yRes; i++){
                free(xtable[i]);
        }
        free(xtable);
}
void array2D::getSize(int &xResolution, int &yResolution){
        xResolution = xRes;
        yResolution = yRes;
}
void array2D::setValue(int col, int row, float val){
        xtable[row][col] = val;
}
float array2D::getValue(int col, int row){
        return(xtable[row][col]);
}

// Child Class of array2D
PGMImage::PGMImage(int xResolution, int yResolution, char* imageFilename)
        : array2D(xResolution,yResolution){
        strcpy(filename, imageFilename);

}
void PGMImage::getResolution(int &xResolution, int &yResolution){
        getSize(xResolution, yResolution);
}
void PGMImage::setPixel(int x,int y,float val){
        setValue(x,y,val);
}
float PGMImage::getPixel(int x,int y){
        return(getValue(x,y));
}
void PGMImage::writeFile(){
        //Create the file and write to it
        FILE *fout = fopen(filename, "w");

        // Header
        fprintf(fout, "%s\n%d %d\n%d\n", "P2", xRes,yRes, 255);

        // Loop and write the table
        for(int i = 0; i < yRes; i++){
                for(int j = 0; j < xRes; j++){
                        fprintf(fout, "%0.f ", xtable[i][j]);
                }
                fprintf(fout, "\n");
        }
        // Close the file
        fclose(fout);
}

ComplexNumber::ComplexNumber(double i,double j){
        real = i;
        imag = j;
}
ComplexNumber ComplexNumber::add(ComplexNumber c2){
        real = real + c2.real;
        imag = imag + c2.imag;
        return(ComplexNumber(real,imag));
}
ComplexNumber ComplexNumber::squared(){
        double tmpReal = real;
        real = (real * real) - (imag * imag);
        imag = (tmpReal * imag) + (imag * tmpReal);
        return(ComplexNumber(real,imag));
}
double ComplexNumber::abs(){
        return(sqrt(pow(real,2) + pow(imag,2)));
}
void ComplexNumber::prt(){
        printf("%f, %f\n",real, imag);
}

int main(int argc, char const *argv[]) {


        // initialize Local Variables
        double cxmin = -0.111;
        double cymin = 0.624;
        double cxmax = -0.062;
        double cymax = 0.673;
        double real, imag;
        int iter;
        int maxIter = 255;

        //Populate the 2d array
        int k = 0;
        while(k < 24){
                // Create the a new image of 640 by 480
                char fname[2048];
                snprintf(fname, sizeof(char) * 32, "fractal%i.pgm", k);
                printf("%s\n", fname);
                PGMImage *img = new PGMImage(1000,1000, fname);
                int xRes, yRes;

                // Set the resolution
                img->getResolution(xRes, yRes);
                for(int y = 0; y < yRes; y++){
                        for(int x = 0; x < xRes; x++){
                                // Get the values of the complex number
                                real = cxmin +  (x/(xRes-1.0))*(cxmax - cxmin);
                                imag = cymin + (y/(yRes-1.0))*(cymax - cymin);

                                // Create the ComplexNumber
                                ComplexNumber c(real, imag);
                                ComplexNumber z(0,0);
                                iter = 0;

                                // Loop until max iterations
                                while (iter < maxIter && z.abs() < 2) {
                                        //z = c.add(z.squared());
                                        z = (z.squared()).add(c);
                                        iter++;
                                }

                                // Final check if pixel is colored or black
                                if (z.abs() >= 2) {
                                        img->setPixel(x,y,iter);
                                }
                                else{
                                        img->setPixel(x,y,0);
                                }
                        }

                }
                // Write out the PGMImage
                img->writeFile();

                // Update the counter
                k++;

                // Delete the img pointer
                delete img;

                // update the c values to zoom in
                cxmin = cxmin + 0.001;
                cymin = cymin + 0.001;
                cxmax = cxmax - 0.001;
                cymax = cymax - 0.001;
        }

        return 0;
}
