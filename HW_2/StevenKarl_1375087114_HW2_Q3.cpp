// Steven Karl


//Libraries
#include <iostream>
#include "Q3.h"
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
        // Create the a new image of 640 by 480
        char fname[] = "fractal.pgm";
        PGMImage *img = new PGMImage(640,480, fname);
        int xRes, yRes;

        // Set the resolution
        img->getResolution(xRes, yRes);

        // initialize Local Variables
        double cxmin = -2;
        double cymin = -1;
        double cxmax = 1;
        double cymax = 1;
        double xRange = cxmax - cxmin;
        double yRange = cymax - cymin;
        double real, imag;
        int iter;
        int maxIter = 255;

        //Populate the 2d array
        for(int y = 0; y < yRes; y++){
                for(int x = 0; x < xRes; x++){
                        // Get the values of the complex number
                        real = cxmin + (x/(xRes-1.0))*(xRange);
                        imag = cymin + (y/(yRes-1.0))*(yRange);

                        // Create the ComplexNumbers
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

        // Delete the pointer
        delete img;
        return 0;
}
