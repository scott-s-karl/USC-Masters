// Steven Karl

//Libraries
#include <fstream>
#include <iostream>


//Class declaration/Definition
class charMatrix{
private:
        char **xtable;
        int xRes;
        int yRes;
public:
        charMatrix(int xR, int yR, char c){
                // Define xtable
                xRes = xR;
                yRes = yR;
                xtable = (char **)calloc(yRes, sizeof(char*));
                for(int i = 0; i < yRes; i++){
                        xtable[i] = (char *)calloc(xRes, sizeof(char));
                }
                for (int i = 0; i < yRes; i++) {
                        for(int j = 0; j < xRes; j++){
                                xtable[i][j] =  c;
                        }
                }
        }
        ~charMatrix(){
                for(int i = 0; i < yRes; i++) {
                        free(xtable[i]);
                }
                free(xtable);
        }
        void setChar(int x, int y, char v){
                xtable[y][x] = v;
        }
        char getChar(int x, int y){
                return(xtable[y][x]);
        }
        void prt(){
                for (int i = 0; i < yRes; i++) {
                        for (int j = 0; j < xRes; j++) {
                                printf("%c ", xtable[i][j]);
                        }
                        printf("\n");
                }
        }
        void saveToFile(std::string fileName){
                // Define the file output
                const char * c = fileName.c_str();

                FILE *fout = fopen(c, "w");

                // Loop and add
                for (int i = 0; i < yRes; i++) {
                        for (int j = 0; i < xRes; j++) {
                                fprintf(fout, "%c ", xtable[i][j]);
                        }
                        fprintf(fout, "\n");
                }
                fclose(fout);

        }
 };

 int main(int argc, char const *argv[]) {

        int xR=25;
        int yR=25;
        int myChar='x';
        charMatrix c(xR,yR,' '); // fill a 25x25 block with blanks
        for(int i=0;i < yR;i++) {
                for(int j=0;j < xR;j++) {
                        /*if(i%2 == 0 && j%2 ==0){
                                 c.setChar(j,i,myChar);
                        }
                        if(i%2 != 0 && j%2 != 0){
                                c.setChar(j,i,myChar);
                        }
                        if(i == j){
                                c.setChar(j,i,myChar);

                        }*/
                        if(i + j < xR){
                                c.setChar(j,i,myChar);

                        }

                }
        }
        c.prt();
        c.saveToFile("fun.txt");

        return 0;
 }
