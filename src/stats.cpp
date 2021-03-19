
#include "stats.hpp"


static double sum_squares(const double data[], const size_t n);
static void my_downheap(double * data, const size_t N, size_t k);


/* stats_mean, stats_variance, stats_kurtosis and stats_skew copied from GSL GNU
 * with some minor changes
 *
 * GSL NOTE: we use a recurrence relation in these functions to stably update a 
 *           running value so there aren't any large sums that can overflow
 */


double stats_mean(const double data[], const size_t size) {
	double mean = 0;

	for (size_t i = 0; i < size; i++)
		mean += (data[i] - mean) / (i + 1);

	return mean;
}


double stats_variance(const double data[], const size_t size, const double mean) {
	double variance = 0;

	for (size_t i = 0; i < size; i++) {
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

	for (size_t i = 0; i < size; i++) {
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

	for (size_t i = 0; i < size; i++) {
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

		if (num < min)
			min = num;
		else if (num > max)
			max = num;
	}
}


void stats_sort(double * data, const size_t n) {
	size_t N;
	size_t k;

	if (n == 0)
		return;			/* No data to sort */

	/* We have n_data elements, last element is at 'n_data-1', first at
	   '0' Set N to the last element number. */

	N = n - 1;

	k = N / 2;
	k++;				/* Compensate the first use of 'k--' */
	do {
		k--;
		my_downheap(data, N, k);
	} while (k > 0);

	while (N > 0) {
		/* first swap the elements */
		double tmp = data[0];
		data[0] = data[N];
		data[N] = tmp;

		/* then process the heap */
		N--;

		my_downheap(data, N, 0);
	}
}


double stats_median_from_sorted_data(const double sorted_data[], const size_t n) {
	double median;
	const size_t lhs = (n - 1) / 2;
	const size_t rhs = n / 2;

	if (n == 0)
		return 0.0;

	if (lhs == rhs) {
		median = sorted_data[lhs];
	} else {
		median = (sorted_data[lhs] + sorted_data[rhs])/2.0;
	}

	return median;
}


double stats_quantile_from_sorted_data(
    const double sorted_data[], const size_t n, const double f
) {
	const double index = f * (n - 1) ;
	const size_t lhs = (int)index ;
	const double delta = index - lhs ;
	double result;

	if (n == 0)
		return 0.0;

	if (lhs == n - 1) {
		result = sorted_data[lhs];
	} else {
		result =   ((1 - delta) * sorted_data[lhs])
		         + (delta * sorted_data[lhs + 1]);
	}

	return result ;
}


double stats_rms(
    const double data_x[], const double data_y[], const double data_z[], 
    const size_t n
) {
	double sum = 0;

	sum += sum_squares(data_x, n);
	sum += sum_squares(data_y, n);
	sum += sum_squares(data_z, n);

	return sqrt(sum / (n*3));
}


/*=========================== ANCILLARY FUNCTIONS ===========================*/


static double sum_squares(const double data[], const size_t n) {
	double sum = 0;

	for (size_t i = 0; i < n; i++) {
		sum += data[i] * data[i];
	}

	return sum;
}


static void my_downheap(double * data, const size_t N, size_t k) {
	double v = data[k];

	while (k <= N / 2) {
		size_t j = 2 * k;

		if (j < N && data[j] < data[j + 1])
			j++;

		if (!(v < data[j]))		/* avoid infinite loop if nan */
			break;

		data[k] = data[j];

		k = j;
	}

	data[k] = v;
}
