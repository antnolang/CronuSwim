
#ifndef _CATCH_HELPER_H_
#define _CATCH_HELPER_H_


#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "config_test.hpp"



float read_expected_from_csv(float features[], const std::string filename);
float read_duration_from_csv(const std::string filename);
bool read_window_from_file(double imu_data[7][W_SIZE], std::ifstream &eventfile);
void read_1st_window_from_csv(
    double imu_data[7][W_SIZE], const std::string filename
);
char movement_int_to_char(int i);


#endif
