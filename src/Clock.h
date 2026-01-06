#pragma once

#include <vector>
#include <string>
#include "raylib.h"

// I realized this clock isn't needed if i just print everything to the console instantly instead
// But I have kept it included in case I need to implement a more realistic time-based environment
struct Clock {
	int currentMilliSecond = 0; // 0 - 100
	int currentSecond = 0; // 0 - 60
	int currentMinute = 0; // 0 - 60
	int currentHour = 0;  // 0 - 24
	int currentDay = 0; // 0 - infinity
	int timeMultiplier = 1;

	// made up clock
	inline void update() {
		currentMilliSecond += timeMultiplier;

		if (currentMilliSecond == 100) {
			currentSecond++;
			currentMilliSecond = 0;
		}
		if (currentSecond == 60) {
			currentMinute++;
			currentSecond = 0;
		}
		if (currentMinute == 60) {
			currentHour++;
			currentMinute = 0;
		}
		if (currentHour == 24) {
			currentHour = 0;
			currentDay++;
		}
	}

	inline void display() {
		std::string format = std::to_string(currentSecond) + "/"
			+ std::to_string(currentMinute) + "/"
			+ std::to_string(currentHour);
		DrawText(format.c_str(), 200, 200, 50, WHITE);
	}
};
