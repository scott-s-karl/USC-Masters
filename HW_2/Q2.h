// Steven Karl

#include <iostream>

// Parent class declaration
class array2D{
public:
        // Constructor
        array2D(int xResolution, int yResolution);

        // Destructor
        ~array2D();
        void getSize(int &xResolution, int &yResolution);
        void setValue(int col, int row, float val);
        float getValue(int col, int row);

protected:
        float **xtable;
        int xRes;
        int yRes;
};

// Child class declaration

class PGMImage: public array2D{
private:
        char filename[2048];
public:
        //Constructor
        PGMImage(int xResolution, int yResolution, char* imageFilename);

        void getResolution(int &xResolution, int &yResolution);
        void setPixel(int x, int y, float val);
        float getPixel(int x, int y);
        void writeFile();

};
