#include <math.h>
#include <stdbool.h>
#include <stdio.h>

//  (a) Begin the definition of a function called min_pos
//  which locates the minimum value in an array and returns the 
//  smallest index of the minimum value in the array.
//  
//  Parameters:
//      values   - an array of double precision floating point values.
//      num_vals - an int which specifies the number of items to process.
//
//  Returns:
//      An int indicating the location at which the minimum may be found:
//      *   If num_vals is 0: return -1 to record the fact that there is
//          no data to process.
//      *   Otherwise: return i in 0 .. (num_vals-1) such that:
//          (For all j in 0..(num_vals-1), values[i] <= values[j]) AND 
//          (For any j where values[j] == values[i], j >= i)
//      i.e.
//          values[i] is the "smallest" value in the array, and i is 
//          the "smallest" index where that happens.

 double min_pos(double values[], int num_vals)
{    
    //  (b) If number of usable items is less than or equal to 0
    //  return -1.
    if(num_vals == 0){
        return -1;
    }
    //  (c) Declare variable store index position, initially 0 
    int index = 0;
    //  (d) Iterate over array, counting from 1
    for(int i = 1; i < num_vals; i++){
        for(j = 0; j >= i, j++){
            //  (e) If item at current position less than item at index
            //  position, replace index position with current position.
            if(values[j] == values[i){
                
            index = i;
            }
            
        }
        
    }

    //  (f) Return index position.
    return index;
}


#define MAX_ITEMS (100)

int main(void)
{
    double items[MAX_ITEMS];
    int array_size;

    // Get number of items.
    printf("Please enter number of items (up to %d) that will be processed: ", MAX_ITEMS);
    scanf("%d", &array_size);

    // if number of items exceeds array size, restrict it to that value.
    if (array_size > MAX_ITEMS)
    {
        array_size = MAX_ITEMS;
    }

    for (int i = 0; i < array_size; i++)
    {
        printf("Please enter (floating point) item %d of %d: ", (i + 1), array_size);
        scanf("%lf", &items[i]);
    }

    int result = min_pos(items, array_size);

    printf("The index of the minimum is %d.\n", result);

    return 0;
}
