#include <stdio.h>

void count_down( void ) {
	// (a) Print the introductory message.
	printf("Reverse order list of integers greater than 77, beginning at 395, stepping by -6.\n");
	// (b) Declare an integer variable that you will use to count. Initially 
	//     the counter should equal the start value, 395.
	int counter = 395;

	// (c) Begin a WHILE statement that will execute its loop body if the 
	//     counter is greater than the end value, 77.
	while(counter>77){
		// (e) Print the value of the counter on a line by itself.
		printf("%d\n", counter);
		
		// (f) Subtract the step size, 6, from the counter.
		counter -= 6;
	}
	// (d) End the WHILE statement
	return;
}

int main( void ) {
	count_down();
	return 0;
}