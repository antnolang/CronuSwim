
#include "catch.hpp"
#include "stats.hpp"


const double margin_error = 0.0000001;


TEST_CASE("stats from subset of data", "[stats]") {
	// Random data generated from https://codinglab.huostravelblog.com/programming/random-number-generator/index.php
	const double testing_data[100] = {-429.11096,762.7724301,-944.4189971,808.4863868,498.7000974,-1147.7893021,-0.2229277,1465.8864741,299.1545391,1692.8306344,-751.1683531,-1400.4061996,-511.9834614,-456.8869582,-1044.5026333,-966.5049714,1011.3246529,-1591.0382994,360.2895839,-1985.959844,39.172051,1310.3503965,-147.3852191,-1102.050044,-1267.5420634,589.4954183,-760.5763248,277.2071244,-197.5331606,-1740.3748209,328.455919,1337.3944697,-1862.9889167,1885.7178031,-1243.5859617,-47.1813264,393.8360503,-689.6364064,-1866.9198384,117.309042,6.3044812,-970.9134378,71.4853466,825.9346334,673.4234781,-1146.7830288,112.9383353,1992.6278992,748.9365271,-1043.6869169,1142.7998284,115.3242464,938.1569948,1422.4864905,-596.7539877,-204.1367473,-536.8688215,-200.730688,1698.4228503,-62.0332023,-1354.8601381,-1067.7413275,330.4452059,577.24019,229.8049253,-90.3810667,-560.4426217,-1300.6597155,-1876.1631488,128.0051013,1074.9436702,1056.1185371,-883.482972,-408.0050169,836.8338486,1708.914581,-857.9079356,912.3536298,1847.2883571,-247.3189464,-126.0271237,-232.9789571,1189.5500834,199.7920079,145.774729,183.9308415,-38.4398814,429.3587922,156.2614783,1386.8711467,-52.628085,-1045.6605321,-860.9229808,118.0121573,1008.5195908,-560.7200453,831.4141548,-1458.4217251,-9.814853,-1656.5587969};

	SECTION("subset [10,50)") {
		const double * data = testing_data + 10;
		const size_t size = 40;
		const double mean = -267.8351243575, var = 1051151.1960538,
			     sd = 1025.2566488708;
		double min, max;

		CHECK(stats_mean(data, size) == Approx(mean).margin(margin_error));
		CHECK(stats_variance(data, size, mean) == Approx(var).margin(margin_error));
		CHECK(stats_sd(var) == Approx(sd).margin(margin_error));
		CHECK(stats_kurtosis(data, size, mean, sd) == Approx(-0.667603486532236).margin(margin_error));
		CHECK(stats_skew(data, size, mean, sd) == Approx(0.26269141940770846).margin(margin_error));

		stats_min_max(min, max, data, size);
		CHECK(min == Approx(-1985.959844).margin(margin_error));
		CHECK(max == Approx(1992.6278992).margin(margin_error));
	}
	SECTION("subset [67,100)") {
		const double * data = testing_data + 67;
		const size_t size = 33;
		const double mean = 48.431272466667, var = 893650.99054591, 
			     sd = 945.33115390635;
		double min, max;

		CHECK(stats_mean(data, size) == Approx(mean).margin(margin_error));
		CHECK(stats_variance(data, size, mean) == Approx(var).margin(margin_error));
		CHECK(stats_sd(var) == Approx(sd).margin(margin_error));
		CHECK(stats_kurtosis(data, size, mean, sd) == Approx(-0.6591495084927934).margin(margin_error));
		CHECK(stats_skew(data, size, mean, sd) == Approx(-0.10769088031586398).margin(margin_error));

		stats_min_max(min, max, data, size);
		CHECK(min == Approx(-1876.1631488).margin(margin_error));
		CHECK(max == Approx(1847.2883571).margin(margin_error));
	}
}

