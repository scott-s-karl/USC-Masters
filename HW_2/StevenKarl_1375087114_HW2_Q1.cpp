// Steven Karl


#include <iostream>


class array2D{
public:
        // Constructor
        array2D(int xResolution, int yResolution){
                xRes = xResolution;
                yRes = yResolution;
                // New version
                //xtable = new float*[yRes];

                // calloc version
                xtable = (float **)calloc(yRes, sizeof(*xtable));
                for(int i = 0; i < yRes; i++){

                        // Calloc version
                        xtable[i] = (float *)calloc(xRes, sizeof(float));

                        //New version
                        //xtable[i] = new float[xRes];
                }
        }

        // Destructor
        ~array2D(){
                // Freeing the memory starting with the subarrays
                for(int i = 0; i < yRes; i++){
                        free(xtable[i]);
                }
                // free the first pointer
                free(xtable);
        }
        void getSize(int &xResolution, int &yResolution){
                xResolution = xRes;
                yResolution = yRes;
        }
        void setValue(int col, int row, float val){
                xtable[row][col] = val;
        }
        float getValue(int col, int row){
                return(xtable[row][col]);
        }

protected:
        float **xtable;
        int xRes;
        int yRes;
};

int main(int argc, char const *argv[]) {
        array2D *a = new array2D(320,240);
        int xRes, yRes;

        // a -> getSize is the same as (*a).getSize()
        a->getSize(xRes,yRes);

        //NOTE the j,i ordering below (twice), since we specify coords as (x,y) [not (y,x)]
        for(int i=0;i < yRes;i++){
                for(int j=0;j < xRes;j++){
                        a->setValue(j,i,100); // constant value of 100 at all locations
                }
        }


        for(int i=0;i < yRes;i++){
                for(int j=0;j < xRes;j++){
                        printf("%.0f ",a->getValue(j,i));
                }
                printf("\n");
        }

        delete a;
        return 0;
}
