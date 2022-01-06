// Steven Karl
// Read a file and find occurances

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[]) {

        char in_name[80];
        FILE *in_file;
        char word[50];
        int c = 0;

        printf("Enter the name of the file:\n");
        scanf("%s", in_name);

        in_file = fopen(in_name, "r");

        if(in_file == NULL){
                printf("Can't open %s for reading. \n", in_name);
        }
        else{
                while(fscanf(in_file, "%s", word) != EOF){
                        if(strstr(word,"Alice")){
                                printf("%s\n", word);
                                c++;
                        }

                }
                fclose(in_file);
        }
        printf("%d\n", c);
        return 0;
}
