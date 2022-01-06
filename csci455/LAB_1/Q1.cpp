// Steven Karl
// Homework # 1 - Q1
// 5-26-2021
//--------------


//Libraries
//--------------
#include <iostream>
#include <math.h>


int factorial(int n){
        int total = n;
        for (int i = n; i > 1; i--) {
                total = total * (i-1);
        }
        return(total);
}

int main(int argc, char const *argv[]) {

        // Function call
        std::cout << factorial(10) << '\n';

        return 0;
}
