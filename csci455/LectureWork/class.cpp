#include <iostream>

// Declaration
class Vector{
private:
        float x,y,z;
public:
        void setVec(float a, float b, float c);
        void prtVec();
        void dot(Vector f){
                std::cout << (x*f.x + y*f.y + z*f.z) << '\n';
        }
};

//Definition -- Return Type Constructor::Function name(inputs){}
void Vector::setVec(float a, float b, float c){
        x = a;
        y = b;
        z = c;
}
void Vector::prtVec(){
        printf("%.2f %.2f %.2f", x,y,z);
        std::cout << "\n----------" << '\n';
}

int main(int argc, char const *argv[]) {
        Vector v(1,2,3);
        v.setVec(1,2,3);
        v.prtVec();

        Vector u;
        u.setVec(0.5,0.5,0.5);
        u.prtVec();

        u.dot(v);
        v.dot(u);

        return 0;
}
