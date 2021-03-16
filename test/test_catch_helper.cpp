
#include "catch.hpp"
#include "catch_helper.hpp"


template <typename T>
constexpr void check_array_approx(
    const T computed[], const T expected[], const int length
) {
	for (int i = 0; i < length; i++)
		CHECK(computed[i] == Approx(expected[i]).margin(0.000001));
}



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
