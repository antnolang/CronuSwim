
#include "process_data.hpp"
#include "trained_model.h" // it's a header file only: can only be imported once


constexpr int feats_count = 43; // average timestamp, 21 accel stats, 21 gyros stats
constexpr int feats_sensor_count = (feats_count - 1) / 2;



void process_window(
    const double imu_data[7][W_SIZE], std::vector<movement> &windows, 
    std::vector<double> &timestamps
) {
	float features[feats_count];
	movement w_class;

	extract_features(features, imu_data);
	// classify using features and skipping timestamp
	w_class = (movement)eml_bayes_predict(&trained_model_model, features + 1, 
					      feats_count - 1);

	windows.push_back(w_class);
	timestamps.push_back(features[0]);
}


void extract_features(float features[], const double imu_data[7][W_SIZE]) {
	constexpr int p_offset = feats_sensor_count + 1;

	features[0] = stats_mean(imu_data[0], W_SIZE);
	extract_stats_from_sensor(features + 1, ACCEL, imu_data);
	extract_stats_from_sensor(features + p_offset, GYROS, imu_data);
}


void extract_stats_from_sensor(
    float * features, const sensor_3D sensor, const double imu_data[7][W_SIZE]
) {
	const int x = sensor, y = sensor + 1, z = sensor + 2;

	const double mean_x = stats_mean(imu_data[x], W_SIZE);
	const double mean_y = stats_mean(imu_data[y], W_SIZE);
	const double mean_z = stats_mean(imu_data[z], W_SIZE);
	const double var_x = stats_variance(imu_data[x], W_SIZE, mean_x);
	const double var_y = stats_variance(imu_data[y], W_SIZE, mean_y);
	const double var_z = stats_variance(imu_data[z], W_SIZE, mean_z);
	const double stdev_x = stats_sd(var_x);
	const double stdev_y = stats_sd(var_y);
	const double stdev_z = stats_sd(var_z);
	double min_x, max_x, min_y, max_y, min_z, max_z;
	stats_min_max(min_x, max_x, imu_data[x], W_SIZE);
	stats_min_max(min_y, max_y, imu_data[y], W_SIZE);
	stats_min_max(min_z, max_z, imu_data[z], W_SIZE);
	const double kurt_x = stats_kurtosis(imu_data[x], W_SIZE, mean_x, stdev_x);
	const double kurt_y = stats_kurtosis(imu_data[y], W_SIZE, mean_y, stdev_y);
	const double kurt_z = stats_kurtosis(imu_data[z], W_SIZE, mean_z, stdev_z);
	const double skew_x = stats_skew(imu_data[x], W_SIZE, mean_x, stdev_x);
	const double skew_y = stats_skew(imu_data[y], W_SIZE, mean_y, stdev_y);
	const double skew_z = stats_skew(imu_data[z], W_SIZE, mean_z, stdev_z);

	//const float rms = sqrt(np.square(subset_data.to_numpy()).sum() / 
	//		     (W_SIZE*3)); 
	// too complex and not significative at all

	// gsl needs to rearrange the data to get this stats. Ignored by now
	/*
	const float median_x = gsl_stats_float_median(data_x, 1, W_SIZE);
	const float median_y = gsl_stats_float_median(data_y, 1, W_SIZE);
	const float median_z = gsl_stats_float_median(data_z, 1, W_SIZE);
	const float q1_x = 
	       gsl_stats_float_quantile_from_sorted_data(data_x, 1, W_SIZE, 0.25);
	const float q1_y =  
	       gsl_stats_float_quantile_from_sorted_data(data_y, 1, W_SIZE, 0.25);
	const float q1_z =  
	       gsl_stats_float_quantile_from_sorted_data(data_z, 1, W_SIZE, 0.25);
	const float q3_x =  
	       gsl_stats_float_quantile_from_sorted_data(data_x, 1, W_SIZE, 0.75);
	const float q3_y =  
	       gsl_stats_float_quantile_from_sorted_data(data_y, 1, W_SIZE, 0.75);
	const float q3_z =  
	       gsl_stats_float_quantile_from_sorted_data(data_z, 1, W_SIZE, 0.75);
	const float iqr_x = q3_x - q1_x;
	const float iqr_y = q3_y - q1_y;
	const float iqr_z = q3_z - q1_z;
	*/

	const double result[feats_sensor_count] = {
		mean_x,  mean_y,  mean_z,  //median_x, median_y, median_z, 
		stdev_x, stdev_y, stdev_z, var_x,    var_y,    var_z,
		min_x,   min_y,   min_z,   max_x,    max_y,    max_z,
		kurt_x,  kurt_y,  kurt_z,  skew_x,   skew_y,   skew_z,
		//q1_x,    q1_y,    q1_z,    q3_x,     q3_y,     q3_z, 
		//iqr_x,   iqr_y,   iqr_z,   rms
	};
	const double * p_result = result;

	// copying result into features
	int length = feats_sensor_count;
	while (length > 0) {
		*features = (float)*p_result;
		features++; p_result++;
		length--;
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

