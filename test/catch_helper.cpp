
#include "catch_helper.hpp"


bool read_window_from_file(
    double imu_data[7][W_SIZE], std::ifstream &eventfile
) {
	int i = 0;
	std::string str, number;
	std::vector<std::string> row;

	while (i < W_SIZE && std::getline(eventfile, str)) {
		row.clear();
		std::istringstream iss(str);

		while (std::getline(iss, number, ','))
			row.push_back(number);

		imu_data[0][i] = stod(row[0]); // t
		imu_data[1][i] = stod(row[1]); // aX
		imu_data[2][i] = stod(row[2]); // aY
		imu_data[3][i] = stod(row[3]); // aZ
		imu_data[4][i] = stod(row[4]); // gX
		imu_data[5][i] = stod(row[5]); // gY
		imu_data[6][i] = stod(row[6]); // gZ

		i++;
	}

	return i == W_SIZE;
}


float read_duration_from_csv(const std::string filename) {
	float duration;
	std::string str, number;
	std::vector<double> start_end;
	std::vector<std::string> row;

	std::ifstream eventfile("./test/data/" + filename);
	if (!eventfile.is_open())
		std::exit(ENOENT);

	std::getline(eventfile, str); // skip the CSV header
	while (std::getline(eventfile, str)) {
		row.clear();
		std::istringstream iss(str);
		while (std::getline(iss, number, ','))
			row.push_back(number);

		if (row[0].find("+") != std::string::npos)
			start_end.push_back(stod(row[0]));
	}

	duration = start_end.size() == 2 ? start_end[1]-start_end[0] : -1.0;

	return duration;
}


void read_1st_window_from_csv(
    double imu_data[7][W_SIZE], const std::string filename
) {
	std::string str;
	std::vector<std::string> row;

	std::ifstream eventfile("./test/data/" + filename);
	if (!eventfile.is_open())
		std::exit(ENOENT);

	std::getline(eventfile, str); // skip the CSV header
	read_window_from_file(imu_data, eventfile);
}


float read_expected_from_csv(float features[], const std::string filename) {
	float timestamp_expected;
	std::string str, number;

	std::ifstream statsfile("./test/data/" + filename);
	if (!statsfile.is_open())
		std::exit(ENOENT);

	// skip the CSV header
	std::getline(statsfile, str);

	// read data line
	std::getline(statsfile, str);
	std::istringstream iss(str);
	int i = 0;
	while (std::getline(iss, number, ',')) {
		if (i == 0)
			timestamp_expected = stof(number);			
		else
			features[i-1] = stof(number);

		i++;
	}

	return timestamp_expected;
}


char movement_int_to_char(int i) {
	char res;

	if (i == 0)
		res = 'S';
	else if (i == 2)
		res = 'F';
	else
		res = 'O';

	return res;
}
