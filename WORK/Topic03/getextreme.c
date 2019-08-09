#include <stdio.h>

//  (a) Begin the definition a function called Min that returns the
//      minimum of its three double precision floating point 
//      arguments.
// 
//      For all triples of double precision floating point values (x, y, z) the 
//      function must satisfy the following conditions:
//          ((Min(x,y,z) = x) OR (Min(x,y,z) = y OR (Min(x,y,z) = z))
//      AND (Min(x,y,z) <= x)
//      AND (Min(x,y,z) <= y)
//      AND (Min(x,y,z) <= z)

double Min (double x, double y, double z){
    double min;

    if (x<= y && x<=z){
        min =x;
    } 
    else if ( y<= z && y<=z){
        min = y;
    } 
    else { //if (z<=x && z<=y){
        min = z;
    }

    //  (b) Implement the logic required to calculate the minimum
    //      of the three input values, and return the result.
    return min;
    }

int main(void) {
    double x, y, z, minVal;

    printf("Please enter three numeric values: ");
    scanf("%lf%lf%lf", &x, &y, &z);
    minVal = Min(x, y, z);
    printf("Min(%0.10f, %0.10f, %0.10f) = %0.10f\n", x, y, z, minVal);

    return 0;
}
