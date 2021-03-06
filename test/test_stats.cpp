
#include "catch.hpp"
#include "stats.hpp"


static void stats_check_unsorted(
    const double * data, const int size, const double stats[8]
);
static void stats_check_sorted(
    double data[], const double sorted_data[], const int size, 
    const double stats[3]
);



TEST_CASE("stats from subset of data without sorting", "[stats][unit_test]") {
	// Random data generated from https://codinglab.huostravelblog.com/programming/random-number-generator/index.php
	constexpr double testing_data[100] = {-429.11096,762.7724301,-944.4189971,808.4863868,498.7000974,-1147.7893021,-0.2229277,1465.8864741,299.1545391,1692.8306344,-751.1683531,-1400.4061996,-511.9834614,-456.8869582,-1044.5026333,-966.5049714,1011.3246529,-1591.0382994,360.2895839,-1985.959844,39.172051,1310.3503965,-147.3852191,-1102.050044,-1267.5420634,589.4954183,-760.5763248,277.2071244,-197.5331606,-1740.3748209,328.455919,1337.3944697,-1862.9889167,1885.7178031,-1243.5859617,-47.1813264,393.8360503,-689.6364064,-1866.9198384,117.309042,6.3044812,-970.9134378,71.4853466,825.9346334,673.4234781,-1146.7830288,112.9383353,1992.6278992,748.9365271,-1043.6869169,1142.7998284,115.3242464,938.1569948,1422.4864905,-596.7539877,-204.1367473,-536.8688215,-200.730688,1698.4228503,-62.0332023,-1354.8601381,-1067.7413275,330.4452059,577.24019,229.8049253,-90.3810667,-560.4426217,-1300.6597155,-1876.1631488,128.0051013,1074.9436702,1056.1185371,-883.482972,-408.0050169,836.8338486,1708.914581,-857.9079356,912.3536298,1847.2883571,-247.3189464,-126.0271237,-232.9789571,1189.5500834,199.7920079,145.774729,183.9308415,-38.4398814,429.3587922,156.2614783,1386.8711467,-52.628085,-1045.6605321,-860.9229808,118.0121573,1008.5195908,-560.7200453,831.4141548,-1458.4217251,-9.814853,-1656.5587969};

	SECTION("subset [10,50)") {
		constexpr int size = 40;
		const double * data = testing_data + 10;
		// mean, var, sd, kurt, skew, min, max, rms
		constexpr double stats[8] = {-267.8351243575, 1051151.1960538, 
			1025.2566488708, -0.667603486532236, 0.26269141940770846, 
			-1985.959844, 1992.6278992, 1059.6635550463245};

		stats_check_unsorted(data, size, stats);
	}
	SECTION("subset [67,100)") {
		constexpr int size = 33;
		const double * data = testing_data + 67;
		// mean, var, sd, kurt, skew, min, max, rms
		constexpr double stats[8] = {48.431272466667, 893650.99054591, 
			945.33115390635, -0.6591495084927934, 
			-0.10769088031586398, -1876.1631488, 1847.2883571, 
			946.5709580895957};

		stats_check_unsorted(data, size, stats);
	}
}


TEST_CASE("stats from subset of sorted data", "[stats][unit_test]") {
	// Random data generated from https://codinglab.huostravelblog.com/programming/random-number-generator/index.php
	double testing_data[100] = {-429.11096,762.7724301,-944.4189971,808.4863868,498.7000974,-1147.7893021,-0.2229277,1465.8864741,299.1545391,1692.8306344,-751.1683531,-1400.4061996,-511.9834614,-456.8869582,-1044.5026333,-966.5049714,1011.3246529,-1591.0382994,360.2895839,-1985.959844,39.172051,1310.3503965,-147.3852191,-1102.050044,-1267.5420634,589.4954183,-760.5763248,277.2071244,-197.5331606,-1740.3748209,328.455919,1337.3944697,-1862.9889167,1885.7178031,-1243.5859617,-47.1813264,393.8360503,-689.6364064,-1866.9198384,117.309042,6.3044812,-970.9134378,71.4853466,825.9346334,673.4234781,-1146.7830288,112.9383353,1992.6278992,748.9365271,-1043.6869169,1142.7998284,115.3242464,938.1569948,1422.4864905,-596.7539877,-204.1367473,-536.8688215,-200.730688,1698.4228503,-62.0332023,-1354.8601381,-1067.7413275,330.4452059,577.24019,229.8049253,-90.3810667,-560.4426217,-1300.6597155,-1876.1631488,128.0051013,1074.9436702,1056.1185371,-883.482972,-408.0050169,836.8338486,1708.914581,-857.9079356,912.3536298,1847.2883571,-247.3189464,-126.0271237,-232.9789571,1189.5500834,199.7920079,145.774729,183.9308415,-38.4398814,429.3587922,156.2614783,1386.8711467,-52.628085,-1045.6605321,-860.9229808,118.0121573,1008.5195908,-560.7200453,831.4141548,-1458.4217251,-9.814853,-1656.5587969};

	SECTION("subset [10,50)") {
		constexpr int offset = 10;
		constexpr int size = 50 - offset;
		double * data = testing_data + offset;
		constexpr double sorted_data[size] = {-1985.959844,-1866.9198384,-1862.9889167,-1740.3748209,-1591.0382994,-1400.4061996,-1267.5420634,-1243.5859617,-1146.7830288,-1102.050044,-1044.5026333,-1043.6869169,-970.9134378,-966.5049714,-760.5763248,-751.1683531,-689.6364064,-511.9834614,-456.8869582,-197.5331606,-147.3852191,-47.1813264,6.3044812,39.172051,71.4853466,112.9383353,117.309042,277.2071244,328.455919,360.2895839,393.8360503,589.4954183,673.4234781,748.9365271,825.9346334,1011.3246529,1310.3503965,1337.3944697,1885.7178031,1992.6278992};
		// median, q1, q3
		constexpr double stats[3] = {-172.45918985, -1058.889485975, 
					      368.67620050000005};

		stats_check_sorted(data, sorted_data, size, stats);
	}
	SECTION("subset [67,100)") {
		constexpr int offset = 67;
		constexpr int size = 100 - offset;
		double * data = testing_data + offset;
		constexpr double sorted_data[size] = {-1876.1631488,-1656.5587969,-1458.4217251,-1300.6597155,-1045.6605321,-883.482972,-860.9229808,-857.9079356,-560.7200453,-408.0050169,-247.3189464,-232.9789571,-126.0271237,-52.628085,-38.4398814,-9.814853,118.0121573,128.0051013,145.774729,156.2614783,183.9308415,199.7920079,429.3587922,831.4141548,836.8338486,912.3536298,1008.5195908,1056.1185371,1074.9436702,1189.5500834,1386.8711467,1708.914581,1847.2883571};
		// media, q1, q3
		constexpr double stats[3] = {118.0121573, -560.7200453, 
					     836.8338486};

		stats_check_sorted(data, sorted_data, size, stats);
	}
}


/* =========================  ANCILLARY FUNCTIONS  ========================= */


static void stats_check_unsorted(
    const double * data, const int size, const double stats[8]
) {
	constexpr double me = 0.0000001;
	double stats_min, stats_max;
	const double mean = stats[0], var = stats[1], sd = stats[2], 
		     kurt = stats[3], skew = stats[4], min = stats[5], 
		     max = stats[6], rms = stats[7];

	CHECK(stats_mean(data, size) == Approx(mean).margin(me));
	CHECK(stats_variance(data, size, mean) == Approx(var).margin(me));
	CHECK(stats_sd(var) == Approx(sd).margin(me));
	CHECK(stats_kurtosis(data, size, mean, sd) == Approx(kurt).margin(me));
	CHECK(stats_skew(data, size, mean, sd) == Approx(skew).margin(me));
	CHECK(stats_rms(data, data, data, size) == Approx(rms).margin(me));

	stats_min_max(stats_min, stats_max, data, size);
	CHECK(stats_min == Approx(min).margin(me));
	CHECK(stats_max == Approx(max).margin(me));
}


static void stats_check_sorted(
    double data[], const double sorted_data[], const int size, 
    const double stats[3]
) {
	constexpr double me = 0.0000001;
	const double median = stats[0], q1 = stats[1], q3 = stats[2];

	stats_sort(data, size);

	for (int i = 0; i < size; i++) {
		CHECK(data[i] == Approx(sorted_data[i]));
	}
	
	CHECK(stats_median_from_sorted_data(data, size) == Approx(median).margin(me));
	CHECK(stats_quantile_from_sorted_data(data, size, 0.25) == Approx(q1).margin(me));
	CHECK(stats_quantile_from_sorted_data(data, size, 0.75) == Approx(q3).margin(me));
}
