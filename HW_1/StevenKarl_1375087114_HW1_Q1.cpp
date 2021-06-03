// Steven Karl
// Homework # 1 - Q1
// 5-26-2021
//--------------


//Libraries
//--------------
#include <iostream>
#include <math.h>

double mySqrt(double x){
        // mySqrt(4) should return 2
        double estimate = 1.0;
        double newEstimate = 0.5*(estimate + (x/estimate));

        //Loop until precision reached
        while(abs(newEstimate - estimate) > 0.0000000000001){
                // reset estimate
                estimate = newEstimate;

                // recalculate new estimate
                newEstimate = 0.5*(estimate + (x/estimate));

        }
        return(newEstimate);

}


int main(int argc, char const *argv[]) {

        // Call the function
        double s = mySqrt(5);

        // Print the output
        std::cout << s << std::endl; // expect 2.236

        return 0;
}
