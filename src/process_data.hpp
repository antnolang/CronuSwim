#ifndef _PROCESS_DATA_H_
#define _PROCESS_DATA_H_


#include <vector>
#include <cmath>

#include "stats.hpp"

enum movement { START, OTHER, FINISH };
enum sensor_3D { ACCEL = 1, GYROS = 4 };

double process_data(std::vector<double> imu_data[7]);
void extract_features(
    float features[], const std::vector<double> imu_data[7], const int first_pos
);
void extract_stats_from_sensor(
    float * features, const sensor_3D sensor, 
    const std::vector<double> imu_data[7], const int first_pos
);
double estimate_time(
    const std::vector<movement> &windows, const std::vector<double> &timestamps
);


#endif
