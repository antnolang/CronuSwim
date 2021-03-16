
#include <fstream>
#include <vector>
#include <string>

#include "catch.hpp"
#include "config_test.hpp"
#include "catch_helper.hpp"
#include "process_data.hpp"
#include "stats.hpp"



static void read_data_from_csv(
    std::vector<movement> &windows, std::vector<double> &timestamps, 
    const std::string filename
);



/* ================================ USE CASES ================================*/

// TODO: how to run use_cases tests after process_data unit testing after stats 
//       unit testing? Order tests so that dependencies are tested before the 
//	 other tests
TEST_CASE("process_data for every real_event file: expected <= 5 percentage of "
	  "failures with a margin of 1 sec", "[use_case]"
) {
	int fail_count = 0;

	for (int i = 1; i <= REAL_EVENT_FILE_COUNT; i++) {
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

	REQUIRE((float)fail_count / (float)REAL_EVENT_FILE_COUNT <= 0.05f);
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
	constexpr int files_per_phases_count = EVENT_FILE_COUNT;

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


/* =========================  ANCILLARY FUNCTIONS  ========================= */


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
