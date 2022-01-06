// Steven Karl
// Homework # 1 - Q3
// 5-26-2021
//--------------


//Libraries
//--------------
#include <iostream>
#include <ctime>
#include <math.h>

void print_stars(int n){
        for (int i = 0; i < n; i++) {
                std::cout << "*";
        }
        std::cout << '\n';
}

void display_paretoGraph(){
        // Seed the rand function
        srand(time(NULL));

        // Local variables
        int c = 0;
        int pNorm;
        double x;
        double r;
        double paretoValue;
        double largestValue = -10000000;
        double smallestValue = 10000000;
        int binArr[10] = {0};
        double a = 8;
        int numAss = 0;

        // Initial Print
        std::cout << '\n';
        std::cout << "Pareto Distribution Graph" << '\n';
        std::cout << "-------------------------" << '\n';

        // Finding the min and the max
        for (size_t i = 0; i < 1000; i++) {

                r = (double) rand()/RAND_MAX;

                // Calculate pareto value
                x = 1 / pow(r,(1/a));

                // Pareto Value
                paretoValue = a*(pow(x,-a-1));

                // Check against small and large
                if(paretoValue < smallestValue){
                        smallestValue = paretoValue;
                }
                if(paretoValue > largestValue){
                        largestValue = paretoValue;
                }

        }

        // Re seed the values
        srand(time(NULL));

        // Loop through and bin the values
        for (size_t i = 0; i < 1000; i++) {
                r = (double) rand()/RAND_MAX;

                // Calculate pareto value
                x = 1 / pow(r,(1/a));

                // Pareto Value
                paretoValue = a*(pow(x,-a-1));

                // Normaize the value
                pNorm = floor((paretoValue - smallestValue)/(largestValue - smallestValue)*10);

                // bin it
                binArr[pNorm]++;
        }

        // Loop and print
        for (const double &val : binArr) {
                std::cout << c << "-";
                numAss = floor(100*(val/1000));
                print_stars(numAss);
                std::cout << '\n';
                c++;
        }

}

int main(int argc, char const *argv[]) {

        // Call display function
        display_paretoGraph();
        return 0;
}
