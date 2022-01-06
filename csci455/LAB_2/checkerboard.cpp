// Steven Karl
//

//Libraries
#include <iostream>
#include <fstream>



void draw_checkerboard(){
        int xRes = 256;
        int yRes = 256;

        // Create the double pointer
        int **board = (int **)calloc(xRes, sizeof(int*));

        // Loop to get the other columns
        for(int i = 0; i < xRes; i++){
                board[i] = (int *)calloc(yRes, sizeof(int));
        }

        for (int i = 0; i < xRes; i++) {
                for (int j = 0; j < yRes; j++) {
                        if((i/(xRes/8))%2==0){
                                if((j/(yRes/8))%2==0){
                                        board[i][j] = 255;
                                }
                                else{
                                        board[i][j] = 0;
                                }
                        }
                        else{
                                if((j/(yRes/8))%2==0){
                                        board[i][j] = 0;
                                }
                                else{
                                        board[i][j] = 255;
                                }
                        }
                }
        }

        for (int i = 0; i < xRes; i++) {
                for (int j = 0; j < yRes; j++) {
                        printf("%d ", board[i][j]);
                }
                printf("\n");
        }
        // Create and open the file to write to
        FILE *fout = fopen("myImg.pgm", "w");

        //header
        fprintf(fout, "%s\n%d %d\n%d\n", "P2", xRes, yRes, 255);
        for (int i = 0; i < xRes; i++) {
                for (int j = 0; j < yRes; j++) {
                        fprintf(fout, "%d ", board[i][j]);
                }
                fprintf(fout, "\n");
        }
        fclose(fout);

}


int main(int argc, char const *argv[]) {
        /* code */
        draw_checkerboard();
        return 0;
}
