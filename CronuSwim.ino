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

	change_status_to('W', "Waiting for reading signal:");
}


void loop() {
	read_button_input();

	switch (status) {
	case 'W':
		if (push_button)
			change_status_to('R', "Reading IMU data ...");

		break;
	case 'R': {
		double imu_data[7][W_SIZE];

		read_imu_data(imu_data);
		process_window(imu_data, windows, timestamps);

		if (push_button)
			change_status_to('P', "Processing data ...");

		break; }
	case 'P': {
		float estimate = estimate_time(windows, timestamps);

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
		change_status_to('W', "Waiting for reading signal:");

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

	for (int i = 0; i < W_SIZE; i++) {
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
		}
	}
}


void change_status_to(char new_status, String serial_message) {
	push_button = false;
	status = new_status;
	Serial.println(serial_message);
}
