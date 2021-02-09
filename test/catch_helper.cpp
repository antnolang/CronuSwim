
#include "catch_helper.hpp"


double read_data_from_csv(std::vector<double> imu_data[7], std::string filename) {
	double t, aX, aY, aZ, gX, gY, gZ, expected_duration;
	std::string str, number;
	std::vector<double> start_end;
	std::vector<std::string> row;

	std::ifstream eventfile("./test/data/" + filename);
	if (!eventfile.is_open()) { std::exit(ENOENT); }

	std::getline(eventfile, str); // skip the CSV header
	while (std::getline(eventfile, str)) {
		row.clear();
		std::istringstream iss(str);
		while (std::getline(iss, number, ',')) {
			row.push_back(number);
		}

		t  = stod(row[0]);
		aX = stod(row[1]);
		aY = stod(row[2]);
		aZ = stod(row[3]);
		gX = stod(row[4]);
		gY = stod(row[5]);
		gZ = stod(row[6]);

		imu_data[0].push_back(t);
		imu_data[1].push_back(aX);
		imu_data[2].push_back(aY);
		imu_data[3].push_back(aZ);
		imu_data[4].push_back(gX);
		imu_data[5].push_back(gY);
		imu_data[6].push_back(gZ);

		if (row[0].find("+") != std::string::npos) {
			start_end.push_back(t);
		}
	}

	if (start_end.size() != 2) {
		expected_duration = -1.0;
	} else {
		expected_duration = start_end[1] - start_end[0];
	}

	return expected_duration;
}

