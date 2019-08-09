#include <stdio.h>

//  (a) Begin the definition a function called minimum which returns
//      the minimum of its two double precision floating point arguments.
// 
//      For all pairs of double precision floating point values x and y, the 
//      function must satisfy the following conditions:
//          ((minimum(x,y) = x) OR (minimum(x,y) = y))
//      AND (minimum(x,y) <= x)
//      AND (minimum(x,y) <= y)

double minimum(double x, double y)
{
    //  (b) If the value of the first argument is less than or equal to that of the
    //      second argument: return the value of the first argument.
    double min;

    if (x<= y){
        min =x;
    }
    // (c) Otherwise: return the value of the second argument.
    else{
        min = y;
    } 
    
    return min;

}

int main(void) {
    double x, y, minVal;

    printf("Please enter two numeric values: ");
    scanf("%lf%lf", &x, &y);
    minVal = minimum(x, y);
    printf( "minimum(%0.10f, %0.10f) = %0.10f\n", x, y, minVal);

    return 0;
}
