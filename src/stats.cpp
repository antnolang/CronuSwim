
#include "stats.hpp"



/* stats_mean, stats_variance, stats_kurtosis and stats_skew copied from GSL GNU
 *
 * NOTE: we use a recurrence relation in these functions to stably update a 
 *       running value so there aren't any large sums that can overflow
 */

double stats_mean(const double data[], const size_t size) {
	double mean = 0;
	size_t i;

	for (i = 0; i < size; i++) {
		mean += (data[i] - mean) / (i + 1);
	}

	return mean;
}

double stats_variance(const double data[], const size_t size, const double mean) {
	double variance = 0;
	size_t i;

	for (i = 0; i < size; i++) {
		const double delta = (data[i] - mean);
		variance += (delta * delta - variance) / (i + 1);
	}

	return variance;
}

double stats_sd(const double variance) {
	return sqrt(variance);
}

double stats_kurtosis(
    const double data[], const size_t size, const double mean, const double sd
) {
	double avg = 0, kurtosis;
	size_t i;

	for (i = 0; i < size; i++) {
		const double x = (data[i] - mean) / sd;
		avg += (x * x * x * x - avg)/(i + 1);
	}

	kurtosis = avg - 3.0;

	return kurtosis;
}

double stats_skew(
    const double data[], const size_t size, const double mean, const double sd
) {
	double skew = 0;
	size_t i;

	for (i = 0; i < size; i++) {
		const double x = (data[i] - mean) / sd;
		skew += (x * x * x - skew) / (i + 1);
	}

	return skew;
}

void stats_min_max(
    double &min, double &max, const double data[], const size_t size
) {
	double num;

	min = data[0];
	max = data[0];

	for (size_t i = 0; i < size; i++) {
		num = data[i];

		if (num < min) {
			min = num;
		} else if (num > max) {
			max = num;
		}
	}
}

