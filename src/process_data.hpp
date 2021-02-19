
#ifndef _PROCESS_DATA_H_
#define _PROCESS_DATA_H_

#undef W_SIZE
#define W_SIZE 70


#include <vector>
#include <cmath>

#include "stats.hpp"


enum movement { START, OTHER, FINISH };
enum sensor_3D { ACCEL = 1, GYROS = 4 };



void process_window(
    const double imu_data[7][W_SIZE], std::vector<movement> &windows, 
    std::vector<double> &timestamps
);
void extract_features(float features[], const double imu_data[7][W_SIZE]);
void extract_stats_from_sensor(
    float * features, const sensor_3D sensor, const double imu_data[7][W_SIZE]
);
float estimate_time(
    const std::vector<movement> &windows, const std::vector<double> &timestamps
);


#endif

