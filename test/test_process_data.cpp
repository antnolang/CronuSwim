
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "catch.hpp"
#include "process_data.hpp"
#include "stats.hpp"


constexpr int event_file_count = 70;
constexpr int real_event_file_count = 30;


// Ancillary functions
static void read_1st_window_from_csv(
    double imu_data[7][W_SIZE], const std::string filename
);
static float read_expected_from_csv(float features[], const std::string filename);
static char movement_int_to_char(int i);
static void read_data_from_csv(
    std::vector<movement> &windows, std::vector<double> &timestamps, 
    const std::string filename
);
static bool read_window_from_file(
    double imu_data[7][W_SIZE], std::ifstream &eventfile
);
static float read_duration_from_csv(const std::string filename);

template <typename T>
static constexpr void check_array_approx(
    const T computed[], const T expected[], const int length
) {
	for (int i = 0; i < length; i++)
		CHECK(computed[i] == Approx(expected[i]).margin(0.000001));
}


/* ================================ USE CASES ================================*/

// TODO: how to run use_cases tests after process_data unit testing after stats 
//       unit testing? Order tests so that dependencies are tested before the 
//	 other tests
TEST_CASE("process_data for every real_event file: expected <= 5 percentage of "
	  "failures with a margin of 1 sec", "[use_case]"
) {
	int fail_count = 0;

	for (int i = 1; i <= real_event_file_count; i++) {
		std::vector<movement> windows;
		std::vector<double> timestamps;
		const std::string filename = "real_event" + std::to_string(i) + 
					     ".csv";

		read_data_from_csv(windows, timestamps, filename);
		const float estimation = estimate_time(windows, timestamps);
		const float duration = read_duration_from_csv(filename);

		if (estimation != Approx(duration).margin(0.475)) {
			WARN("failed estimation of file " << filename << ": " 
			     << estimation << " == " << duration);

			fail_count++;
		}
	}

	REQUIRE((float)fail_count / (float)real_event_file_count <= 0.05f);
}


/* =============================  UNIT TESTING  ============================= */


TEST_CASE("extract_stats_from_sensor using first window of event27.csv", 
	  "[process_data]"
) {
	constexpr float me = 0.0000001;
	double imu_data[7][W_SIZE]; // w_size = 70
	sensor_3D sensor = GENERATE(ACCEL, GYROS);
	const int length = sensor == ACCEL ? FEATS_ACCEL_COUNT : FEATS_GYROS_COUNT;
	float features[length];
	float all_expected[FEATS_COUNT-1];
	
	read_1st_window_from_csv(imu_data, "event27.csv");
	read_expected_from_csv(all_expected, "stats_event27.csv");

	// all_expected[FEATS_COUNT-1]:
	//     [0, FEATS_ACCEL_COUNT)   --> ACCEL
	//     [FEATS_ACCEL_COUNT, end) --> GYROS
	const int i_offset = sensor == ACCEL ? 0 : FEATS_ACCEL_COUNT;

	const std::string sensor_text = sensor == ACCEL ? "ACCEL" : "GYROS";
	DYNAMIC_SECTION("using the following sensor: " << sensor_text) {
		extract_stats_from_sensor(features, sensor, imu_data);		

		for (int i = 0; i < length; i++) {
			CHECK(features[i] == Approx(all_expected[i_offset + i]).margin(me));
		}
	}
}


TEST_CASE("extract_features using first window of event27.csv", "[process_data]") {
	constexpr float me = 0.0000001;
	double imu_data[7][W_SIZE]; // w_size = 70
	float features[FEATS_COUNT];
	float expected_res[FEATS_COUNT-1];

	read_1st_window_from_csv(imu_data, "event27.csv");
	const float expected_timestamp = read_expected_from_csv(expected_res, 
								"stats_event27.csv");

	extract_features(features, imu_data);

	CHECK(features[0] == Approx(expected_timestamp).margin(me));
	for (int i = 1; i < FEATS_COUNT; i++)
		CHECK(features[i] == Approx(expected_res[i-1]).margin(me));
}


TEST_CASE("estimate_time", "[process_data]") {
	constexpr float me = 0.0000001;

	// timestamps generated randomly from https://onlinenumbertools.com/generate-random-numbers
	// and sorted with https://onlinenumbertools.com/sort-numbers

	SECTION("windows vector structure: S") {
		std::vector<movement> windows { START };
		std::vector<double> timestamps { 119.5313000 };
		const float estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(-1.0).margin(me));
	}
	SECTION("windows vector structure: S-O") {
		std::vector<movement> windows { START, OTHER };
		std::vector<double> timestamps { 109.0248433, 150.3003813 };
		const float estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(-1.0).margin(me));
	}
	SECTION("windows vector structure: S-F") {
		std::vector<movement> windows { START, FINISH };
		std::vector<double> timestamps { 89.4012356, 105.4825108 };
		const float estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(16.0812752).margin(me));
	}
	SECTION("windows vector structure: S-O-F") {
		std::vector<movement> windows { START, OTHER, FINISH };
		std::vector<double> timestamps { 120.1440555, 124.5974776, 170.0856107 };
		const float estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(49.9415552).margin(me));
	}
	SECTION("windows vector structure: S-S-S-O-O-F") {
		std::vector<movement> windows { START, START, START, OTHER, OTHER, FINISH };
		std::vector<double> timestamps { 37.4677422, 72.0910316, 105.8747562, 116.9008569, 145.2582483, 176.4821443 };
		const float estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(104.67096763).margin(me));
	}
	SECTION("windows vector structure: S-O-O-F-F-F") {
		std::vector<movement> windows { START, OTHER, OTHER, FINISH, FINISH, FINISH };
		std::vector<double> timestamps { 10.3808606, 16.5270632, 31.639025, 48.8453378, 129.1268248, 138.8565136 };
		const float estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(95.22869813).margin(me));
	}
	SECTION("windows vector structure: S-S-O-O-O-F-F-F") {
		std::vector<movement> windows { START, START, OTHER, OTHER, OTHER, FINISH, FINISH, FINISH };
		std::vector<double> timestamps { 69.9216319, 88.759334, 94.3151542, 97.0999855, 108.7473468, 163.9695802, 178.0340261, 180.1993829 };
		const float estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(94.72718012).margin(me));
	}
	SECTION("windows vector structure: O-F-F-F") {
		std::vector<movement> windows { OTHER, FINISH, FINISH, FINISH };
		std::vector<double> timestamps { 25.4465651, 26.060263, 103.8528839, 119.7795445 };
		const float estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(-1.0).margin(me));
	}
	SECTION("windows vector structure: O-O-S-O-O-O-F-F") {
		std::vector<movement> windows { OTHER, OTHER, START, OTHER, OTHER, OTHER, FINISH, FINISH };
		std::vector<double> timestamps { 28.5234834, 41.290292, 42.3668828, 44.6222175, 66.1084976, 66.9544599, 127.176117, 161.2673861 };
		const float estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(101.85486875).margin(me));
	}
	SECTION("windows vector structure: S-S-S-S-O-F-F-O-O-O") {
		std::vector<movement> windows { START, START, START, START, OTHER, FINISH, FINISH, OTHER, OTHER, OTHER };
		std::vector<double> timestamps { 8.3324093, 27.5179932, 30.6220361, 36.9015075, 43.079359, 65.995949, 132.0058814, 137.7501647, 154.0399874, 177.7511424 };
		const float estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(73.15742868).margin(me));
	}
	SECTION("windows vector structure: O-S-S-O-O-F-F-F-F-O") {
		std::vector<movement> windows { OTHER, START, START, OTHER, OTHER, FINISH, FINISH, FINISH, FINISH, OTHER };
		std::vector<double> timestamps { 37.8360314, 62.7220482, 72.7768973, 125.4473504, 126.7777332, 130.3964639, 156.3557188, 176.9320942, 188.5169835, 195.524165 };
		const float estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(95.30084235).margin(me));
	}
	SECTION("empty windows") {
		std::vector<movement> windows;
		std::vector<double> timestamps;
		const float estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(-2.0).margin(me));
	}
}


TEST_CASE("process_window for 70 files per phases: expected success rate higher " 
	  "than 0.95", "[process_data]"
) {
	int success_count = 0;
	std::vector<movement> windows;
	std::vector<double> timestamps;
	constexpr float me = 0.0000001;
	constexpr int phases_count = 3;
	constexpr int files_per_phases_count = event_file_count;

	for (int i = 0; i < phases_count; i++) { // Start, other, finish
		for (int j = 1; j <= files_per_phases_count; j++) {
			double imu_data[7][W_SIZE];
			const char phase = movement_int_to_char(i);

			const std::string filename = phase + std::to_string(j) 
							   + ".csv";
			read_1st_window_from_csv(imu_data, filename);

			process_window(imu_data, windows, timestamps);

			if (windows.back() != (movement)i) {
				WARN("failed classification of " << filename << 
				     ": \n - predicted = " << 
				     movement_int_to_char(windows.back()) << 
				     "\n - actual = " << movement_int_to_char(i));
			} else {
				const double expected = stats_mean(imu_data[0], W_SIZE);
				CHECKED_IF(timestamps.back() == Approx(expected).margin(me)) {
					success_count++;
				}
			}
		}
	}

	const float success_rate = (float)success_count / 
				   (phases_count*files_per_phases_count);
	REQUIRE(success_rate > 0.95);
}


/* =====================  ANCILLARY FUNCTION AND TESTS  ===================== */


// Functions -------------------------------------------------------------------

static bool read_window_from_file(
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


static void read_data_from_csv(
    std::vector<movement> &windows, std::vector<double> &timestamps, 
    const std::string filename
) {
	std::string str;
	std::vector<std::string> row;
	int skip_factor = 4; // TODO: document this factor

	std::ifstream eventfile("./test/data/" + filename);
	if (!eventfile.is_open())
		std::exit(ENOENT);

	std::getline(eventfile, str); // skip the CSV header
	double imu_data[7][W_SIZE];
	while (read_window_from_file(imu_data, eventfile)) {
		process_window(imu_data, windows, timestamps);

		for (int i = 0; i < skip_factor; i++) {
			std::getline(eventfile, str); // skipping 4 lines after each window
		}
	}
}


static float read_duration_from_csv(const std::string filename) {
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


static void read_1st_window_from_csv(
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


static float read_expected_from_csv(float features[], const std::string filename) {
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


static char movement_int_to_char(int i) {
	char res;

	if (i == 0)
		res = 'S';
	else if (i == 2)
		res = 'F';
	else
		res = 'O';

	return res;
}



// Tests -----------------------------------------------------------------------


TEST_CASE("getting duration from event27.csv", "[catch_helper]") {
	float duration = read_duration_from_csv("event27.csv");

	REQUIRE(duration == Approx(458.0).margin(0.0000001));
}


TEST_CASE("reading first window from event27.csv", "[catch_helper]") {
	double imu_data[7][W_SIZE]; // w_size = 70

	read_1st_window_from_csv(imu_data, "event27.csv");

	// values copied directly from event27.csv
	constexpr double  t[W_SIZE] = {113159.0, 113160.0, 113161.0, 113162.0, 113163.0, 113164.0, 113165.0, 113166.0, 113167.0, 113168.0, 113169.0, 113170.0, 113171.0, 113172.0, 113173.0, 113174.0, 113175.0, 113176.0, 113177.0, 113178.0, 113179.0, 113180.0, 113181.0, 113182.0, 113183.0, 113184.0, 113185.0, 113186.0, 113187.0, 113188.0, 113189.0, 113190.0, 113191.0, 113192.0, 113193.0, 113194.0, 113195.0, 113196.0, 113197.0, 113198.0, 113199.0, 113200.0, 113201.0, 113202.0, 113203.0, 113204.0, 113205.0, 113206.0, 113207.0, 113208.0, 113209.0, 113210.0, 113211.0, 113212.0, 113213.0, 113214.0, 113215.0, 113216.0, 113217.0, 113218.0, 113219.0, 113220.0, 113221.0, 113222.0, 113223.0, 113224.0, 113225.0, 113226.0, 113227.0, 113228.0};
	constexpr double aX[W_SIZE] = {-0.041, -0.043, -0.042, -0.047, -0.052, -0.059, -0.068, -0.069, -0.062, -0.055, -0.047, -0.041, -0.038, -0.037, -0.039, -0.041, -0.037, -0.034, -0.035, -0.036, -0.041, -0.044, -0.045, -0.049, -0.047, -0.041, -0.033, -0.028, -0.027, -0.030, -0.033, -0.035, -0.038, -0.040, -0.040, -0.040, -0.040, -0.043, -0.048, -0.051, -0.053, -0.050, -0.046, -0.042, -0.039, -0.039, -0.042, -0.046, -0.052, -0.051, -0.048, -0.042, -0.033, -0.029, -0.030, -0.034, -0.034, -0.034, -0.037, -0.038, -0.036, -0.038, -0.040, -0.040, -0.045, -0.052, -0.055, -0.052, -0.046, -0.041};
	constexpr double aY[W_SIZE] = {-0.551, -0.563, -0.564, -0.560, -0.558, -0.556, -0.554, -0.551, -0.546, -0.542, -0.540, -0.538, -0.537, -0.541, -0.547, -0.549, -0.552, -0.548, -0.547, -0.548, -0.548, -0.548, -0.544, -0.543, -0.543, -0.542, -0.545, -0.548, -0.549, -0.551, -0.555, -0.556, -0.553, -0.545, -0.542, -0.545, -0.549, -0.550, -0.552, -0.549, -0.546, -0.543, -0.546, -0.553, -0.561, -0.561, -0.556, -0.553, -0.555, -0.556, -0.554, -0.552, -0.549, -0.546, -0.544, -0.542, -0.545, -0.552, -0.561, -0.573, -0.580, -0.575, -0.568, -0.559, -0.553, -0.555, -0.560, -0.570, -0.572, -0.570};
	constexpr double aZ[W_SIZE] = {-0.857, -0.854, -0.850, -0.847, -0.847, -0.848, -0.847, -0.847, -0.844, -0.840, -0.840, -0.844, -0.846, -0.845, -0.845, -0.843, -0.841, -0.841, -0.840, -0.837, -0.839, -0.844, -0.844, -0.847, -0.852, -0.855, -0.855, -0.852, -0.849, -0.851, -0.851, -0.850, -0.844, -0.842, -0.840, -0.840, -0.841, -0.842, -0.849, -0.851, -0.854, -0.853, -0.848, -0.844, -0.840, -0.840, -0.841, -0.845, -0.850, -0.851, -0.849, -0.843, -0.840, -0.840, -0.841, -0.844, -0.842, -0.836, -0.835, -0.839, -0.843, -0.847, -0.843, -0.842, -0.839, -0.841, -0.841, -0.841, -0.842, -0.838};
	constexpr double gX[W_SIZE] = {-0.549, -0.122, -0.671, -0.854, -0.854, -1.465, -1.953, -2.380, -2.869, -2.869, -2.625, -2.502, -2.625, -2.502, -2.380, -2.625, -2.563, -2.563, -2.502, -1.953, -1.465, -1.282, -0.854, -0.854, -0.610, -0.183, -0.122, -0.061, -0.427, -0.732, -0.854, -1.526, -2.136, -2.563, -2.441, -1.770, -1.343, -1.099, -0.732, -0.671, -1.221, -1.465, -1.587, -1.892, -1.770, -1.709, -1.953, -2.014, -1.953, -1.587, -1.587, -1.221, -1.404, -1.465, -1.770, -2.014, -2.075, -1.770, -1.892, -1.831, -1.709, -2.075, -1.953, -1.770, -1.465, -0.916, -0.916, -1.221, -1.831, -2.625};
	constexpr double gY[W_SIZE] = {-2.136, -2.136, -1.709, -1.343, -0.183, 0.977, 1.770, 1.953, 1.770, 1.404, 1.099, 0.732, 0.549, 0.610, 0.793, 1.160, 1.221, 0.854, 0.427, -0.122, -0.488, -0.427, 0.061, 0.610, 1.099, 1.465, 1.343, 0.916, 0.366, -0.061, -0.061, 0.305, 1.038, 1.404, 1.343, 1.160, 1.221, 0.732, 0.122, -0.183, -0.122, 0.244, 0.305, 0.244, 0.122, -0.061, 0.000, 0.122, 0.305, 0.854, 1.648, 1.709, 0.793, -0.122, -0.854, -0.977, -0.671, -0.244, 0.366, 0.549, 0.305, 0.122, 0.305, 0.488, 0.427, 0.183, 0.427, 0.977, 1.160, 1.282};
	constexpr double gZ[W_SIZE] = {0.000, -0.122, -0.122, -0.183, -0.183, -0.061, 0.366, 0.366, 0.061, 0.061, -0.183, -0.244, -0.061, 0.061, 0.427, 0.549, 0.671, 0.854, 0.854, 0.732, 0.549, 0.244, 0.061, 0.244, 0.061, 0.000, -0.061, -0.305, -0.427, -0.183, -0.122, -0.305, -0.671, -1.160, -0.977, -0.732, -0.854, -1.038, -1.343, -1.404, -1.099, -0.977, -0.977, -1.343, -1.587, -1.404, -1.099, -0.732, -0.610, -0.793, -0.916, -1.221, -1.465, -1.770, -1.953, -2.258, -2.625, -2.869, -2.991, -3.052, -2.686, -2.502, -2.319, -1.892, -1.648, -1.465, -1.526, -1.282, -1.221, -1.099};

	check_array_approx(imu_data[0], t , W_SIZE);
	check_array_approx(imu_data[1], aX, W_SIZE);
	check_array_approx(imu_data[2], aY, W_SIZE);
	check_array_approx(imu_data[3], aZ, W_SIZE);
	check_array_approx(imu_data[4], gX, W_SIZE);
	check_array_approx(imu_data[5], gY, W_SIZE);
	check_array_approx(imu_data[6], gZ, W_SIZE);
}


TEST_CASE("reading features from stats_event27.csv", "[catch_helper]") {
	float timestamp;
	float features[FEATS_COUNT-1];
	constexpr float expected_f[FEATS_COUNT-1] = {-0.0425714286,-0.844757143,-0.551000000,-0.844000000,0.00859164283,0.0000836548980,-0.580000000,-0.857000000,-0.537000000,0.882678966,-0.459710829,-0.848750000,-0.0370000000,-0.841000000,0.0100000000,0.0100000000,0.583270614,-1.59731429,0.422014286,-1.70900000,0.427000000,0.725666817,0.856105526,0.526592330,0.732916671,-2.86900000,-2.13600000,-0.0610000000,0.854000000,-0.472314392,0.251614251,-0.0457500000,-0.961750000,1.12950000,1.35993069};
	constexpr float expected_t = 113193.5;

	timestamp = read_expected_from_csv(features, "stats_event27.csv");

	CHECK(timestamp == Approx(expected_t).margin(0.0000001));
	check_array_approx(features, expected_f, FEATS_COUNT-1);
}
