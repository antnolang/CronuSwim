
#include <Arduino_LSM9DS1.h>
#include <vector>

#include "process_data.hpp"


std::vector<double> imu_data[7];
char event_button;



void setup() {
	Serial.begin(9600);
	while (!Serial);

	if (!IMU.begin()) {
		Serial.println("Failed to initialize IMU!");
		while (1);
	}

	event_button = 'W';
	Serial.println("Waiting for reading signal:");
}


void loop() {
	read_button_input();

	if (event_button == 'P') {
		Serial.println("Reading IMU data ...");
		event_button = 'W';
		while(event_button == 'W') {
			read_imu_data();
			read_button_input();
		}

		Serial.println("Processing data ...");
		double estimation = process_data(imu_data);
		if (estimation >= 0.0) {
			Serial.print("Time inferred: ");
			Serial.println(estimation);
		} else if (estimation == -1.0) {
			Serial.println("ERROR: Swimming event not completed");
		} else if (estimation == -2.0) {
			Serial.println("ERROR: Imu data was empty");
		} else {
			Serial.println("ERROR: Unknown error");
		}

		event_button = 'W';
		for (int i = 0; i < 7; i++)
			imu_data[i].clear();

		Serial.println("Waiting for reading signal:");
	}
}



void read_button_input() {
	String in_data = "";

	while (Serial.available() > 0) {
		char recieved = Serial.read();
		in_data += recieved;

		if (recieved == '\n') {
			event_button = in_data[0];
			break;
		}
	}
}


void read_imu_data() {
	if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
		double t;		
		float aX, aY, aZ, gX, gY, gZ; 

		t = millis() / 1000.0; // timestamp in seconds
		IMU.readAcceleration(aX, aY, aZ);
		IMU.readGyroscope(gX, gY, gZ);

		imu_data[0].push_back(t);
		imu_data[1].push_back((double)aX);
		imu_data[2].push_back((double)aY);
		imu_data[3].push_back((double)aZ);
		imu_data[4].push_back((double)gX);
		imu_data[5].push_back((double)gY);
		imu_data[6].push_back((double)gZ);
	}
}
