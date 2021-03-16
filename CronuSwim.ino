/* CronuSwim: https://github.com/antnolang/CronuSwim */


#include <Arduino_LSM9DS1.h>
#include <vector>

#include "process_data.hpp"


char status;
bool push_button;

std::vector<movement> windows;
std::vector<double> timestamps;



void setup() {
	Serial.begin(9600);
	while (!Serial);

	if (!IMU.begin()) {
		Serial.println("Failed to initialize IMU!");
		while (1);
	}

	change_status_to('W');
}


void loop() {
	read_button_input();

	switch (status) {
	case 'W': 	// Waiting
		if (push_button)
			change_status_to('R');

		break;
	case 'R': {	// Reading
		double imu_data[7][W_SIZE];

		read_imu_data(imu_data);
		process_window(imu_data, windows, timestamps);

		if (push_button)
			change_status_to('P');

		break; }
	case 'P': {	// Processing
		const float estimate = estimate_time(windows, timestamps);

		if (estimate >= 0.0) {
			Serial.print("Time inferred: "); Serial.println(estimate);
		} else if (estimate == -1.0) {
			Serial.println("ERROR: Swimming event not completed");
		} else if (estimate == -2.0) {
			Serial.println("ERROR: Nothing to process");
		} else {
			Serial.println("ERROR: Unknown error");
		}

		windows.clear();
		timestamps.clear();
		change_status_to('W');

		break; }
	default:
		Serial.println("ERROR: Unknown status");

		break;
	}
}


void read_button_input() {
	String in_data = "";

	while (Serial.available() > 0) {
		char recieved = Serial.read();
		in_data += recieved;

		if (recieved == '\n' && in_data[0] == 'X') {
			push_button = true;
			break;
		}
	}
}


void read_imu_data(double imu_data[7][W_SIZE]) {
	double t;
	float aX, aY, aZ, gX, gY, gZ;
	int i = 0;

	while (i < W_SIZE) {
		if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {		
			t = millis() / 1000.0; // timestamp in seconds
			IMU.readAcceleration(aX, aY, aZ);
			IMU.readGyroscope(gX, gY, gZ);

			imu_data[0][i] = t;
			imu_data[1][i] = (double)aX;
			imu_data[2][i] = (double)aY;
			imu_data[3][i] = (double)aZ;
			imu_data[4][i] = (double)gX;
			imu_data[5][i] = (double)gY;
			imu_data[6][i] = (double)gZ;

			i++;
		}
	}
}


void change_status_to(const char new_status) {
	push_button = false;
	status = new_status;

	if (new_status == 'W') {
		Serial.println("Waiting for reading signal:");
	} else if (new_status == 'R') {
		Serial.println("Reading data ...");
	} else if (new_status == 'P') {
		Serial.println("Processing data ...");
	}
}
