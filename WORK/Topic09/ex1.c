// Replace 0 with the correct value for each unknown cell.
double pre_scale_001 = 1;
double pre_scale_010 = 8;
double pre_scale_011 = 64;
double pre_scale_100 = 256;
double pre_scale_101 = 1024;

// freq = timer3 bit/prescaler
double counter_frequency_001 = 8000000;
double counter_frequency_010 = 1000000;
double counter_frequency_011 = 125000;
double counter_frequency_100 = 31250;
double counter_frequency_101 = 7812.5;

// Overflow period = 65,536/counter_freq
double overflow_period_001 = 0.008192;
double overflow_period_010 = 0.065536;
double overflow_period_011 = 0.524288;
double overflow_period_100 = 2.097152;
double overflow_period_101 = 8.388608;

// Overflow freq = 1/overflow_period
double overflow_frequency_001 = 122.0703125;
double overflow_frequency_010 = 15.2587890625;
double overflow_frequency_011 = 1.9073486328125;
double overflow_frequency_100 = 0.476837158203125;
double overflow_frequency_101 = 0.1192092895507813;

// There is no test driver for this program.

int main() {
	for ( ;; ) {
	}

	return 0;
}
