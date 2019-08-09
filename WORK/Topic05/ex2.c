#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

//  (Task 1) Declare a new struct type called Date. This structure 
//  could be used to store a calendar date in an application which requires 
//  timekeeping on a daily basis.
// 
//  Members:
//      Year - an int which records the year.
//
//      Month - an int which records the month, with valid values ranging 
//      from 1 to 12.
//
//      Day - an int which records the day in the month, ranging from 1 to
//      31, subject to the rules set out in the documentation comment of the 
//      Date_Valid function.

typedef struct Date
{
    //  (Task 1.1) Declare the fields of the struct in the order listed above.
    int year;
    int month;
    int day;
} Date;

//  (Task 2) Define a function called Date_Read which uses scanf to get the 
//  data for a Date. Fields are to be entered as three separate int values
//  in the format "%d-%d-%d".
// 
//  Parameters:
//      date_ptr - the address of a Date which must be populated by the 
//      function call.
//
//  Returns:
//      The function must return a boolean value indicating the status of the 
//      I/O operation. The status is true if and only if three integer values 
//      have been successfully parsed and saved in date_ptr.
//
//      Do not try to perform other data validation in this function.

bool Date_Read (Date *date_ptr)
{
    //  (Task 2.1) Insert logic to read three integer values from standard input
    //  and save them in the appropriate fields of date_ptr. Use scanf, but do
    //  NOT use printf or any other output function. 
    if(scanf("%d-%d-%d", &date_ptr->year, &date_ptr->month, &date_ptr->day){
        return true;
    }
    return false;
}

//  (Task 3) Define a function called Date_Write which uses printf to 
//  display the value of a Date structure. 
// 
//  Parameters:
//      date - a Date structure that will be displayed.
//
//  Returns:
//      Nothing.

void Date_Write ( Date date )
{
    //  (Task 3.1) Print the horizontal and vertical position of SCREEN-POS_VAR
    //  with format string "%d-%d-%d". Do NOT insert a linefeed.
    printf("%d-%d-%d", date.year, date.month, date. day);
}

//  (Task 4) Define a function called Date_Compare which compares two
//  Date values. Your implementation may assume that these values are 
//  valid dates.
// 
//  Parameters:
//      date1 - a Date structure.
//      date2 - a Date structure.
//
//  Returns:
//      An int which is:
//      -1  if the date represented by date1 is before that represented by 
//          date2;
//      0   if the two values represent the same date;
//      +1 otherwise.

int Date_Compare ( Date date1, Date date2 )
{
    if(date1.year <= date2.year){
        if(date1.month <= date2.month){
            if(date1.day <= date2.day){
                return -1;
            }
        }
    }
    else if (date1.day == date2.day && date1.month == date2.month &&  date1.year == date2.year){
        return 0;
    }
    return 1;
}
 
//  (Task 5) Implement the Date_Valid function which determines if the 
//  supplied date is valid:
//  *   Year must be greater than or equal to 1.
//  *   Month must between 1 and 12 inclusive.
//  *   Day must be at least 1, with upper limits given below:
//      30 days: September, April June, and November.
//      31 days: January, March, May, July, August, October, December.
//      28 or 29: February (usually 28, but 29 in a leap year).
//  
//  A year is a leap year if it is divisible by 400, or if it is
//  divisible by 4 but not divisible by 100.
//  
//  Parameters:
//      date - a Date value.
//
//  Returns:
//      Returns true if and only if the supplied date is valid according to 
//      the definition listed above.

bool Date_Valid ( Date date )
{
    //Check year 
    if(!(date.year >= 1){
        return false;
    }
    
    // Check month 
    if(!(date.month >= 1) && !(date.month <= 12)){
        return false;
    }
    //Check day 
    if(date.day >=1 && date.day <= 31){
        if(date.month == 2){
            if(date.year%4 == 0){
                if (date.year%100 == 0){
                    if(date.year%400 == 0){
                        if(date.day >=1 && date.day <=29){
                            return true;
                        }
                    }else{
                        if(date.day >=1 && date.day <=28){
                            return true;
                        }
                    }
                }else{
                    if(date.day >=1 && date.day <= 29){
                        return true;
                    }
                }
            }else{
                if(date.day >=1 && date.day <=28){
                    return true;
               }
            }
        }else if(date.month == 4 || date.month == 6 || date.month == 9 || date.month == 11){
            if( date.day >=1 && date.day <= 30){
                return true;
            }else{
                return false;
            }
        }else{
            if(date.day >=1 && date.day <=31){
                return true;
            }else{
                return false;
            }
        }
    }
    return true;
}

//  (Task 6) Define a function called Date_Match which compares a query date to
//  the elements of an array of Date objects. The function returns the 
//  address of the first object in the list which satisfies a designated criterion.
// 
//  Parameters:
//      query - a Date structure.
//      dates - an array of Date structures.
//      num_dates - an int which tells the function how many elements there 
//          are in the array.
//      criterion - an int (guaranteed to be -1, 0, or 1) which defines the 
//          matching criterion. 
//
//  Returns:
//      A pointer to a Date object.
//      If num_dates is equal to or less than 0: this value will be NULL.
//      If the query is not valid: this value will be NULL.
//      If there is no valid element x in the array which
//              Date_Compare(x,query) == criterion
//          then this value will be NULL.
//      Otherwise: the return value will be the address of the first valid 
//          Date x in the array for which 
//              Date_Compare(x,query) == criterion.

Date *Date_Match (Date query,Date dates[], int num_dates, int criterion )
{
    if(num_dates <= 0){
        return NULL;
    }
    if(!Date_Valid(query)){
        return NULL;
    }
    
    for(int i = 0; i < num_dates; i++){
        if(Date_Valid(dates[i])){
            if(Date_Compare(dates[i], query) == criterion){
                return &dates[i];
            }
        }  
    }
    return NULL;
}


#define MAX_ITEMS (100)

int main(void)
{
    Date query;
    printf("Input query date using format %s, with year first and day last: ", "%d-%d-%d");
    Date_Read(&query);

    Date ref_dates[MAX_ITEMS] = { {0,0,0} };
    int num_items;

    // Get number of ref_dates.
    printf("Please enter number of items (up to %d) that will be processed: ", MAX_ITEMS);
    scanf("%d", &num_items);

    // if number of ref_dates exceeds array size, restrict it to that value.
    if (num_items > MAX_ITEMS)
    {
        num_items = MAX_ITEMS;
    }

    for (int i = 0; i < num_items; i++)
    {
        printf("Please enter item %d of %d using format %s, with year first and day last: ", (i + 1), num_items, "%d-%d-%d");
        Date_Read(&ref_dates[i]);
    }

    for (int i = 0; i < num_items; i++)
    {
        Date_Write(ref_dates[i]);

        if (!Date_Valid(ref_dates[i]))
        {
            printf(" is not valid.\n");
            continue;
        }

        int cmp = Date_Compare(ref_dates[i], query);
        if (cmp < -1 || cmp > 1)
        {
            printf("Error!!! Date_Compare is broken.\n");
            exit(1);
        }
        char * labels[] = { "less than", "equal to", "greater than" };
        printf(" is %s ", labels[cmp + 1]);
        Date_Write(query);
        printf("\n");
    }

    const int criterion = -1;
    Date * cmp = Date_Match(query, ref_dates, num_items, criterion);

    if (cmp)
    {
        printf("The first valid date matching the search criterion is ");
        Date_Write(*cmp);
    }
    else
    {
        printf("There is no valid date matching the search criterion.\n");
    }

    return 0;
}
