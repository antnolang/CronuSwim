
#include <string>

#include "catch.hpp"
#include "catch_helper.hpp"
#include "process_data.hpp"



TEST_CASE("extract_stats_from_sensor using event27.csv", "[process_data],[static]") {
	constexpr float me = 0.0000001;
	constexpr int feats_sensor_count = 21;
	std::vector<double> imu_data[7];
	float features[feats_sensor_count];
	sensor_3D sensor = GENERATE(ACCEL, GYROS);
	int first_pos = GENERATE(0, 59); // w_size = 70

	constexpr float expected_res[2][2][feats_sensor_count] = {
 /* ACCEL - 0  */	-0.042571428571429, -0.55227142857143, -0.84475714285714, 0.0085916428307173, 0.0091463051534108, 0.0050665167359398, 0.000073816326530616, 0.000083654897959308, 0.000025669591835558, -0.069, -0.580, -0.857, -0.027, -0.537, -0.835, 0.8826789660749395, 0.563486000435029, -0.6289091800652478, -0.8224259442845651, -0.9375635402876765, -0.45971082883459685,
 /* ACCEL - 59 */	-0.034742857142857, -0.56345714285714, -0.8379, 0.0086663317570036, 0.011566806837162, 0.0088964679989436, 0.000075105306122449, 0.00013379102040822, 0.000079147142856227, -0.055, -0.589, -0.855, -0.018, -0.532, -0.820, -0.6349873741886602, 0.3321879782556616, -0.994875225964436, -0.1457982203222088, 0.06111678783540695, 0.17019665034162287,
 /* GYROS - 0  */	-1.5973142857143, 0.42201428571429, -0.77087142857143, 0.7256668173452, 0.85610552575281, 0.9823307403667, 0.52659232979592, 0.73291667122449, 0.96497368346939, -2.869, -2.136, -3.052, -0.061, 1.953, 0.854, -0.7490513065291156, 1.0112479704644457, -0.4723143922086641, 0.25161425088606093, -0.828537796196954, -0.4854193239006291,
 /* GYROS - 59 */	-0.77437142857143, 1.0480571428571, 2.2330142857143, 3.0619488526634, 1.2534808664072, 2.3925670045423, 9.3755307763265, 1.571214282449, 5.7243768712245, -5.432, -0.977, -3.052, 5.493, 3.845, 5.554, -0.6205828634130963, -0.4365929652505005, -0.8253146073298572, 0.4133372071581709, 0.6807733153487078, -0.6037826423274072
	};

	read_data_from_csv(imu_data, "event27.csv");

	const std::string sensor_text = sensor == ACCEL ? "ACCEL" : "GYROS";
	DYNAMIC_SECTION(sensor_text << " as sensor with first_pos=" << first_pos) {
		extract_stats_from_sensor(features, sensor, imu_data, first_pos);

		const int i = sensor == ACCEL ? 0 : 1;
		const int j = first_pos == 0 ? 0 : 1;
		const float (&expected)[feats_sensor_count] = expected_res[i][j];

		for (int i = 0; i < feats_sensor_count; i++) {
			CHECK(features[i] == Approx(expected[i]).margin(me));
		}
	}
}


TEST_CASE("extract_features using event27.csv", "[process_data],[static]") {
	constexpr float me = 0.0000001;
	constexpr int feats_sensor_count = 21;
	constexpr int feats_count = (feats_sensor_count * 2) + 1;
	std::vector<double> imu_data[7];
	float features[feats_count];
	int first_pos = GENERATE(0, 59);

	constexpr double expected_timestamps[2] = {113193.5, 113252.5};
	constexpr float expected_res[2][feats_count-1] = {
 /* ACCEL - 0  */	-0.042571428571429, -0.55227142857143, -0.84475714285714, 0.0085916428307173, 0.0091463051534108, 0.0050665167359398, 0.000073816326530616, 0.000083654897959308, 0.000025669591835558, -0.069, -0.580, -0.857, -0.027, -0.537, -0.835, 0.8826789660749395, 0.563486000435029, -0.6289091800652478, -0.8224259442845651, -0.9375635402876765, -0.45971082883459685,
 /* GYROS - 0  */	-1.5973142857143, 0.42201428571429, -0.77087142857143, 0.7256668173452, 0.85610552575281, 0.9823307403667, 0.52659232979592, 0.73291667122449, 0.96497368346939, -2.869, -2.136, -3.052, -0.061, 1.953, 0.854, -0.7490513065291156, 1.0112479704644457, -0.4723143922086641, 0.25161425088606093, -0.828537796196954, -0.4854193239006291,
 /* ACCEL - 59 */	-0.034742857142857, -0.56345714285714, -0.8379, 0.0086663317570036, 0.011566806837162, 0.0088964679989436, 0.000075105306122449, 0.00013379102040822, 0.000079147142856227, -0.055, -0.589, -0.855, -0.018, -0.532, -0.820, -0.6349873741886602, 0.3321879782556616, -0.994875225964436, -0.1457982203222088, 0.06111678783540695, 0.17019665034162287,
 /* GYROS - 59 */	-0.77437142857143, 1.0480571428571, 2.2330142857143, 3.0619488526634, 1.2534808664072, 2.3925670045423, 9.3755307763265, 1.571214282449, 5.7243768712245, -5.432, -0.977, -3.052, 5.493, 3.845, 5.554, -0.6205828634130963, -0.4365929652505005, -0.8253146073298572, 0.4133372071581709, 0.6807733153487078, -0.6037826423274072
	};

	read_data_from_csv(imu_data, "event27.csv");

	DYNAMIC_SECTION("using first_pos=" << first_pos) {
		extract_features(features, imu_data, first_pos);
		const int i = first_pos == 0 ? 0 : 1;
		const float (&expected)[feats_count-1] = expected_res[i]; // ACCEL and GYROS features

		CHECK(features[0] == Approx(expected_timestamps[i]).margin(me));
		for (int i = 1; i < feats_count; i++)
			CHECK(features[i] == expected[i-1]);
	}
}


TEST_CASE("estimate_time", "[process_data],[static]") {
	constexpr float me = 0.0000001;

	// timestamps generated randomly from https://onlinenumbertools.com/generate-random-numbers
	// and sorted with https://onlinenumbertools.com/sort-numbers

	SECTION("windows vector structure: S") {
		std::vector<movement> windows { START };
		std::vector<double> timestamps { 119.5313000 };
		const double estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(-1.0).margin(me));
	}
	SECTION("windows vector structure: S-O") {
		std::vector<movement> windows { START, OTHER };
		std::vector<double> timestamps { 109.0248433, 150.3003813 };
		const double estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(-1.0).margin(me));
	}
	SECTION("windows vector structure: S-F") {
		std::vector<movement> windows { START, FINISH };
		std::vector<double> timestamps { 89.4012356, 105.4825108 };
		const double estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(16.0812752).margin(me));
	}
	SECTION("windows vector structure: S-O-F") {
		std::vector<movement> windows { START, OTHER, FINISH };
		std::vector<double> timestamps { 120.1440555, 124.5974776, 170.0856107 };
		const double estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(49.9415552).margin(me));
	}
	SECTION("windows vector structure: S-S-S-O-O-F") {
		std::vector<movement> windows { START, START, START, OTHER, OTHER, FINISH };
		std::vector<double> timestamps { 37.4677422, 72.0910316, 105.8747562, 116.9008569, 145.2582483, 176.4821443 };
		const double estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(104.67096763).margin(me));
	}
	SECTION("windows vector structure: S-O-O-F-F-F") {
		std::vector<movement> windows { START, OTHER, OTHER, FINISH, FINISH, FINISH };
		std::vector<double> timestamps { 10.3808606, 16.5270632, 31.639025, 48.8453378, 129.1268248, 138.8565136 };
		const double estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(95.22869813).margin(me));
	}
	SECTION("windows vector structure: S-S-O-O-O-F-F-F") {
		std::vector<movement> windows { START, START, OTHER, OTHER, OTHER, FINISH, FINISH, FINISH };
		std::vector<double> timestamps { 69.9216319, 88.759334, 94.3151542, 97.0999855, 108.7473468, 163.9695802, 178.0340261, 180.1993829 };
		const double estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(94.72718012).margin(me));
	}
	SECTION("windows vector structure: O-F-F-F") {
		std::vector<movement> windows { OTHER, FINISH, FINISH, FINISH };
		std::vector<double> timestamps { 25.4465651, 26.060263, 103.8528839, 119.7795445 };
		const double estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(-1.0).margin(me));
	}
	SECTION("windows vector structure: O-O-S-O-O-O-F-F") {
		std::vector<movement> windows { OTHER, OTHER, START, OTHER, OTHER, OTHER, FINISH, FINISH };
		std::vector<double> timestamps { 28.5234834, 41.290292, 42.3668828, 44.6222175, 66.1084976, 66.9544599, 127.176117, 161.2673861 };
		const double estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(101.85486875).margin(me));
	}
	SECTION("windows vector structure: S-S-S-S-O-F-F-O-O-O") {
		std::vector<movement> windows { START, START, START, START, OTHER, FINISH, FINISH, OTHER, OTHER, OTHER };
		std::vector<double> timestamps { 8.3324093, 27.5179932, 30.6220361, 36.9015075, 43.079359, 65.995949, 132.0058814, 137.7501647, 154.0399874, 177.7511424 };
		const double estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(73.15742868).margin(me));
	}
	SECTION("windows vector structure: O-S-S-O-O-F-F-F-F-O") {
		std::vector<movement> windows { OTHER, START, START, OTHER, OTHER, FINISH, FINISH, FINISH, FINISH, OTHER };
		std::vector<double> timestamps { 37.8360314, 62.7220482, 72.7768973, 125.4473504, 126.7777332, 130.3964639, 156.3557188, 176.9320942, 188.5169835, 195.524165 };
		const double estimation = estimate_time(windows, timestamps);

		CHECK(estimation == Approx(95.30084235).margin(me));
	}
}


TEST_CASE("process_data: expected <5 percentage of failures", "[process_data]") {
	int success_count = 0;
	int fail_count = 0;
	constexpr int test_file_count = 60;

	for (int i = 1; i <= test_file_count; i++) {
		std::vector<double> imu_data[7];
		const std::string filename = "event" + std::to_string(i) + ".csv";

		const double duration = read_data_from_csv(imu_data, filename);
		const double estimation = process_data(imu_data);

		if (estimation == Approx(duration).margin(160.0)) { // TODO: margin 160.0 - low demand
			success_count++;
		} else {
			WARN("failed estimation of file " << filename << ": " 
			     << estimation << " == " << duration);
			fail_count++;
		}
	}

	REQUIRE((float)fail_count / (float)test_file_count < 0.05);
}

