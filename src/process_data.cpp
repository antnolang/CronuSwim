
#include "process_data.hpp"
#include "trained_model.h" // it's a header file only: can only be imported once


static void copy_features(const double * result, float * features, int length);



void process_window(
    double imu_data[7][W_SIZE], std::vector<movement> &windows, 
    std::vector<double> &timestamps
) {
	float features[FEATS_COUNT];
	movement w_class;

	extract_features(features, imu_data);
	// classify using features and skipping timestamp
	w_class = (movement)eml_bayes_predict(&trained_model_model, features + 1, 
					      FEATS_COUNT - 1);

	windows.push_back(w_class);
	timestamps.push_back(features[0]);
}


void extract_features(float features[], double imu_data[7][W_SIZE]) {
	constexpr int p_offset = FEATS_ACCEL_COUNT + 1;

	features[0] = stats_mean(imu_data[0], W_SIZE);
	extract_stats_from_sensor(features + 1, ACCEL, imu_data);
	extract_stats_from_sensor(features + p_offset, GYROS, imu_data);
}


void extract_stats_from_sensor(
    float * features, const sensor_3D sensor, double imu_data[7][W_SIZE]
) {
	const int x = sensor, y = sensor + 1, z = sensor + 2;

	// In common features
	const double mean_x = stats_mean(imu_data[x], W_SIZE);
	const double mean_y = stats_mean(imu_data[y], W_SIZE);
	const double mean_z = stats_mean(imu_data[z], W_SIZE);
	const double var_x = stats_variance(imu_data[x], W_SIZE, mean_x);
	const double var_y = stats_variance(imu_data[y], W_SIZE, mean_y);
	const double var_z = stats_variance(imu_data[z], W_SIZE, mean_z);
	const double stdev_x = stats_sd(var_x);
	const double stdev_z = stats_sd(var_z);
	const double rms = stats_rms(imu_data[x], imu_data[y], imu_data[z], W_SIZE);

	stats_sort(imu_data[x], W_SIZE);
	stats_sort(imu_data[y], W_SIZE);
	stats_sort(imu_data[z], W_SIZE);

	const double median_y = 
			stats_median_from_sorted_data(imu_data[y], W_SIZE);
	const double min_y = imu_data[y][0];
	const double q1_y = 
			stats_quantile_from_sorted_data(imu_data[y], W_SIZE, 0.25);
	const double q3_x = 
			stats_quantile_from_sorted_data(imu_data[x], W_SIZE, 0.75);
	const double q3_y = 
			stats_quantile_from_sorted_data(imu_data[y], W_SIZE, 0.75);
	const double iqr_y = q3_y - q1_y;

	if (sensor == ACCEL) {
		const double kurt_x = 
			stats_kurtosis(imu_data[x], W_SIZE, mean_x, stdev_x);
		const double skew_z = 
			stats_skew(imu_data[z], W_SIZE, mean_z, stdev_z);
		const double min_z = imu_data[z][0];
		const double max_y = imu_data[y][W_SIZE-1];
		const double median_z = 
			stats_median_from_sorted_data(imu_data[z], W_SIZE);
		const double q1_x = 
			stats_quantile_from_sorted_data(imu_data[x], W_SIZE, 0.25);
		const double q1_z = 
			stats_quantile_from_sorted_data(imu_data[z], W_SIZE, 0.25);
		const double q3_z = 
			stats_quantile_from_sorted_data(imu_data[z], W_SIZE, 0.75);
		const double iqr_x = q3_x - q1_x;

		const double result[FEATS_ACCEL_COUNT] = {
			mean_x,  mean_z,  median_y, median_z, stdev_x, var_y, 
			min_y,   min_z,   max_y, kurt_x, skew_z, q1_z, q3_x, 
			q3_z, iqr_x, iqr_y, rms
		};
		copy_features(result, features, FEATS_ACCEL_COUNT);
	} else /* GYROS */ {
		const double stdev_y = stats_sd(var_y);
		const double kurt_z = 
			stats_kurtosis(imu_data[z], W_SIZE, mean_z, stdev_z);
		const double skew_x = 
			stats_skew(imu_data[x], W_SIZE, mean_x, stdev_x);
		const double min_x = imu_data[x][0];
		const double max_x = imu_data[x][W_SIZE-1];
		const double max_z = imu_data[z][W_SIZE-1];
		const double median_x = 
			stats_median_from_sorted_data(imu_data[x], W_SIZE);

		const double result[FEATS_GYROS_COUNT] = {
			mean_x, mean_y, median_x, median_y, stdev_x, stdev_y, 
			var_x, var_y, min_x, min_y, max_x, max_z, kurt_z, 
			skew_x, q1_y, q3_x, iqr_y, rms
		};
		copy_features(result, features, FEATS_GYROS_COUNT);
	}
}


float estimate_time(
    const std::vector<movement> &windows, const std::vector<double> &timestamps
) {
	const size_t length = windows.size();
	size_t last_start_pos = 0, first_finish_pos = 0, j;
	bool start_found = false;
	float result;

	// looking for the last start and the first finish
	for (size_t i = 0; i < length; i++) {
		if (windows[i] == START) {
			last_start_pos = i;
			start_found = true;
		} else if (windows[i] == FINISH && start_found) {
			first_finish_pos = i;
			break;
		}
	}

	if (length == 0) {
		// nothing to process
		result = -2.0;
	} else if (first_finish_pos == 0) {
		// swimming event not completed ==> error code -1.0
		result = -1.0;
	} else {
		// looking for consecutive starts
		int consctv_starts = 1;
		
		j = last_start_pos - 1;
		while (j < SIZE_MAX && windows[j] == START) {
			consctv_starts++; j--;
		}

		// looking for consecutive finishes
		int consctv_finishes = 1;

		j = first_finish_pos + 1;
		while (j < length && windows[j] == FINISH) {
			consctv_finishes++; j++;
		}

		// average timestamps for starts
		const double * p_data, * p_timestamps = timestamps.data();

		p_data = p_timestamps + last_start_pos - consctv_starts + 1;
		const double avg_start_time = stats_mean(p_data, consctv_starts);

		// average timestamps for finishes
		p_data = p_timestamps + first_finish_pos;
		const double avg_finish_time = stats_mean(p_data, 
							  consctv_finishes);

		result = avg_finish_time - avg_start_time;
	}

	return result;
}


/*=========================== ANCILLARY FUNCTIONS ===========================*/


static void copy_features(const double * result, float * features, int length) {
	while (length > 0) {
		*features = (float)*result;
		features++; result++;
		length--;
	}
}
