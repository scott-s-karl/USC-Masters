

#include <iostream>

struct vector {
        float x,y,z;
};

vector add(vector u, vector v){
        vector sum;
        sum.x = u.x + v.x;
        sum.y = u.y + v.y;
        sum.z = u.z + v.z;

        return sum;

}




int main(int argc, char const *argv[]) {
        vector u = {1,0,0};
        vector v = {0,1,0};
        vector s = add(u,v);
        std::cout << s.x << "," << s.y << "," << s.z << '\n';
        return 0;
}
