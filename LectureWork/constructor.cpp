//Steven

#include <iostream>



class row{
private:
        int len;
        int *ptr;
public:
        //constrcutor
        row(int n = 3) {
                len = n;
                ptr = (int *)calloc(len, sizeof(int));
                for (int i = 0; i < len; i++) {
                        ptr[i] = i;
                }
        }
        // destructor
        ~row(){
                free(ptr);
        }
        // print method
        void printRow(char *label);

};

void row::printRow(char *label){
        std::cout << label;
        // print the values in the row
        for (int i = 0; i < len; i++) {
                printf("%d ", ptr[i]);
        }

        // print a new line
        printf("\n");
}

void twoRows(){
        row r;
        row s(100);

        char sLabel[] = "S: ";
        char rLabel[] = "R: ";


        s.printRow(sLabel);
        r.printRow(rLabel);
}

int main(int argc, char const *argv[]) {

        twoRows();
        return 0;
}
