
#include <vector>
#include <string>

#include "catch.hpp"
#include "config_test.hpp"
#include "catch_helper.hpp"
#include "process_data.hpp"


static void read_data_from_csv(
    std::vector<movement> &windows, std::vector<double> &timestamps, 
    const std::string filename
);



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
