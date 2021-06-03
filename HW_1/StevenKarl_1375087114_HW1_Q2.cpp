// Steven Karl
// Homework # 1 - Q2
// 5-26-2021
//--------------


//Libraries
//--------------
#include <iostream>
#include <ctime>
#include <math.h>

// Local Functions
void printStars(int n){

        // Print Loop
        for (size_t i = 0; i < n; i++) {
                std::cout << "*";
        }
}

void display_uniform(){

        // Set up random number calls
        srand(time(NULL));

        // Local Variable Definition
        int binArr[10] = {0};
        double r;
        int j;
        int c = 0;

        std::cout << '\n';
        std::cout << "Uniform Distribution" << '\n';
        std::cout << "--------------------" << '\n';

        // Data Collection Loop
        for (int i = 0; i < 100; i++) {
                r = (double) rand()/RAND_MAX;
                j = floor(r*10);
                if(j == 10){
                        j = 9;
                }
                binArr[j]++;
        }

        // Display Loop
        for (const int &val : binArr) {
                std::cout << c << "-";
                printStars(val);
                std::cout << '\n';
                c++;
        }
        std::cout << '\n';
}

void display_triangular(){

        // Set up random number calls
        srand(time(NULL));

        // Local Variable Definition
        int binArr2[10] = {0};
        double r1,r2,r3;
        int j;
        int c = 0;


        std::cout << "Triangular Distribution" << '\n';
        std::cout << "-----------------------" << '\n';

        // Data Collection Loop
        for (int i = 0; i < 100; i++) {
                r1 = (double) rand()/RAND_MAX;
                r2 = (double) rand()/RAND_MAX;
                r3 = 0.5*(r1 + r2);
                j = floor(r3*10);
                if(j == 10){
                        j = 9;
                }
                binArr2[j]++;
        }

        // Display Loop
        for (const int &val : binArr2) {
                std::cout << c << "-";
                printStars(val);
                std::cout << '\n';
                c++;
        }
}

// Main Routine
int main(int argc, char const *argv[]) {


        // HW Q2 Part 1 - Uniform Distribution
        display_uniform();

        // HW Q2 Part 2 - Triangular Distribution
        display_triangular();

        return 0;
}
