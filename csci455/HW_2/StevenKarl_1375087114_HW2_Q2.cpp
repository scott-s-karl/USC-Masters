// Steven Karl

// Libraries
#include <iostream>
#include <fstream>
#include <cstring>
#include "Q2.h"


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

// Main Routine
int main(int argc, char const *argv[]) {
        char fname[] = "test.pgm";
        PGMImage *a = new PGMImage(256,256,fname);
        int xRes, yRes;

        // a -> getSize is the same as (*a).getSize()
        a->getResolution(xRes,yRes);

        //NOTE the j,i ordering below (twice), since we specify coords as (x,y) [not (y,x)]
        for(int i=0;i < yRes;i++){
                for(int j=0;j < xRes;j++){
                        a->setPixel(j,i,100); // constant value of 100 at all locations
                }
        }


        for(int i=0;i < yRes;i++){
                for(int j=0;j < xRes;j++){
                        printf("%.0f ",a->getPixel(j,i));
                }
                printf("\n");
        }
        a->writeFile();

        delete a;
        return 0;
}
