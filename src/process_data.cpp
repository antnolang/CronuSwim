
#include "process_data.hpp"
#include "trained_model.h" // it's a header file only: can only be imported once


const int feats_count = 43; // average timestamp, 21 accel stats, 21 gyros stats
const int feats_sensor_count = (feats_count - 1) / 2;
const float overlap_factor = 0.25; // window data overlap between them by a factor of 0.25
const int w_size = 70;
const int w_offset = round(w_size*(1.0 - overlap_factor));



double process_data(std::vector<double> imu_data[7]) {
	const int data_size = imu_data[0].size();
	// const int w_count = floor( (data_size - w_size) / 
	//			      (w_size * overlap_factor) ) + 1;
	movement w_class;
	int first_pos, i = 0;
	std::vector<movement> windows;
	std::vector<double> timestamps;
	float features[feats_count];

	while (i + w_size <= data_size) {
		first_pos = i;
		extract_features(features, imu_data, first_pos);
		
		// classify using features and skipping timestamp
		w_class = (movement)eml_bayes_predict(&trained_model_model, 
					              features + 1, 
						      feats_count - 1);

		windows.push_back(w_class);
		timestamps.push_back(features[0]);

		i += w_offset;
	}

	return estimate_time(windows, timestamps);
}


void extract_features(
    float features[], const std::vector<double> imu_data[7], const int first_pos
) {
	const double * time_data = imu_data[0].data();
	int p_offset = feats_sensor_count + 1;

	features[0] = stats_mean(time_data + first_pos, w_size);
	extract_stats_from_sensor(features + 1, ACCEL, imu_data, first_pos);
	extract_stats_from_sensor(features + p_offset, GYROS, imu_data, first_pos);
}


void extract_stats_from_sensor(
    float * features, const sensor_3D sensor, 
    const std::vector<double> imu_data[7], const int first_pos
) {
	const int x = sensor, y = sensor + 1, z = sensor + 2;
	const double * data_x = imu_data[x].data() + first_pos;
	const double * data_y = imu_data[y].data() + first_pos;
	const double * data_z = imu_data[z].data() + first_pos;

	const double mean_x = stats_mean(data_x, w_size);
	const double mean_y = stats_mean(data_y, w_size);
	const double mean_z = stats_mean(data_z, w_size);
	const double var_x = stats_variance(data_x, w_size, mean_x);
	const double var_y = stats_variance(data_y, w_size, mean_y);
	const double var_z = stats_variance(data_z, w_size, mean_z);
	const double stdev_x = stats_sd(var_x);
	const double stdev_y = stats_sd(var_y);
	const double stdev_z = stats_sd(var_z);
	double min_x, max_x, min_y, max_y, min_z, max_z;
	stats_min_max(min_x, max_x, data_x, w_size);
	stats_min_max(min_y, max_y, data_y, w_size);
	stats_min_max(min_z, max_z, data_z, w_size);
	const double kurt_x = stats_kurtosis(data_x, w_size, mean_x, stdev_x);
	const double kurt_y = stats_kurtosis(data_y, w_size, mean_y, stdev_y);
	const double kurt_z = stats_kurtosis(data_z, w_size, mean_z, stdev_z);
	const double skew_x = stats_skew(data_x, w_size, mean_x, stdev_x);
	const double skew_y = stats_skew(data_y, w_size, mean_y, stdev_y);
	const double skew_z = stats_skew(data_z, w_size, mean_z, stdev_z);

	//const float rms = sqrt(np.square(subset_data.to_numpy()).sum() / 
	//		     (w_size*3)); 
	// TODO: too complex and not significative at all

	// TODO: gsl needs to rearrange the data to get this stats. Ignored by now
	/*
	const float median_x = gsl_stats_float_median(data_x, 1, w_size);
	const float median_y = gsl_stats_float_median(data_y, 1, w_size);
	const float median_z = gsl_stats_float_median(data_z, 1, w_size);
	const float q1_x = 
	       gsl_stats_float_quantile_from_sorted_data(data_x, 1, w_size, 0.25);
	const float q1_y =  
	       gsl_stats_float_quantile_from_sorted_data(data_y, 1, w_size, 0.25);
	const float q1_z =  
	       gsl_stats_float_quantile_from_sorted_data(data_z, 1, w_size, 0.25);
	const float q3_x =  
	       gsl_stats_float_quantile_from_sorted_data(data_x, 1, w_size, 0.75);
	const float q3_y =  
	       gsl_stats_float_quantile_from_sorted_data(data_y, 1, w_size, 0.75);
	const float q3_z =  
	       gsl_stats_float_quantile_from_sorted_data(data_z, 1, w_size, 0.75);
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


double estimate_time(
    const std::vector<movement> &windows, const std::vector<double> &timestamps
) {
	const int length = windows.size();
	int last_start_pos = -1, first_finish_pos = -1, i;
	double result;

	// looking for the last start and the first finish
	for (int i = 0; i < length; i++) {
		if (windows[i] == START) {
			last_start_pos = i;
		} else if (windows[i] == FINISH && last_start_pos != -1) {
			first_finish_pos = i;
			break;
		}
	}

	if (last_start_pos == -1 || first_finish_pos == -1) {
		// not completed event of swimming
		result = -1.0;
	} else {
		// looking for consecutive starts
		int consctv_starts = 1;
		
		i = last_start_pos - 1;
		while (i >= 0 && windows[i] == START) {
			consctv_starts++; i--;
		}

		// looking for consecutive finishes
		int consctv_finishes = 1;

		i = first_finish_pos + 1;
		while (i < length && windows[i] == FINISH) {
			consctv_finishes++; i++;
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

