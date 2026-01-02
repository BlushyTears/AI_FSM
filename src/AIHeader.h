//#pragma once
//#include <iostream>
//#include <vector>
//#include <queue>
//#include <string>
//
//#include "raylib.h"
//#include "resource_dir.h"	
//
//struct Clock {
//	int currentMilliSecond = 0; // 0 - 100
//	int currentSecond = 0; // 0 - 60
//	int currentMinute = 0; // 0 - 60
//	int currentHour = 0;  // 0 - 24
//	int currentDay = 0; // 0 - infinity
//	int timeMultiplier = 1;
//
//	inline void update() {
//		currentMilliSecond += timeMultiplier;
//
//		if (currentMilliSecond == 100) {
//			currentSecond++;
//			currentMilliSecond = 0;
//		}
//		if (currentSecond == 60) {
//			currentMinute++;
//			currentSecond = 0;
//		}
//		if (currentMinute == 60) {
//			currentHour++;
//			currentMinute = 0;
//		}
//		if (currentHour == 24) {
//			currentHour = 0;
//			currentDay++;
//		}
//	}
//
//	inline void display() {
//		std::string format = std::to_string(currentSecond) + "/"
//			+ std::to_string(currentMinute) + "/"
//			+ std::to_string(currentHour);
//		DrawText(format.c_str(), 200, 200, 50, WHITE);
//	}
//};
//
//struct MyFSM {
//	enum AgentStates {
//		CollectingMoney,
//		BuyingStuff,
//		EatAndDrink,
//		Socializing
//	};
//	int timeInMinutes = 0;
//	int money = 100;
//	int hunger = 100;
//	int socialScore = 100;
//	AgentStates m_state;
//
//	void work() {
//		money++;
//		std::cout << "Collecting money in job 1";
//	}
//
//	void work2() {
//		money++;
//		std::cout << "Collecting money in job 2";
//	}
//
//	void update() {
//		if (m_state == CollectingMoney) {
//		}
//	}
//};
//
