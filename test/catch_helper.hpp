
#ifndef _CATCH_HELPER_H_
#define _CATCH_HELPER_H_


#include <fstream>
#include <vector>
#include <string>
#include <sstream>



double read_data_from_csv(
    std::vector<double> imu_data[7], const std::string filename
);


#endif

