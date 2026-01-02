#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <queue>
#include <unordered_map>

#include "raylib.h"
#include "resource_dir.h"	

#include "FSMLibrary.h"

// Agent-specific implementation:
template <typename T>
struct Agent;

template <typename T>
struct Agent {
	int id;
	int money = 100;
	std::vector<std::pair<int, std::string>> items;
	int alertness = 100;
	int satiety = 100;
	int socialScore = 100;
	StateMachine<Agent<T>> sm;
};

// Action primitive idea to eat
//template <typename Agent>
//struct EatingAction : Action<Agent> {
//	void execute(Agent& agent) {
//		std::cout << "Hunger currently is " << agent.satiety << " Eating more:";
//		agent.satiety += 5;
//	}
//};

// Idea for scheduling, probably not final thought
//enum EnumStates {
//	CollectingMoney,
//	BuyingStuff,
//	EatAndDrink,
//	Socializing
//};
//
//struct Schedule {
//	// This represents an activity for a particular hour
//	// It should behave like a queue since we forward the hour
//	// Which means we could overwrite our schedule if hunger gets too low
//	std::unordered_map<int, EnumStates> hourlyActivity;
//
//	void requestToScheduleActivity(int _checkForHour, EnumStates _requestedActivity) {
//		// schedule is free
//		if (hourlyActivity.find(_checkForHour) == hourlyActivity.end()) {
//			hourlyActivity[_checkForHour] = _requestedActivity;
//		}
//	}
//
//	void getNextActivity(int _currentHour) {
//		
//	}
//};

//struct Agent {
//	int money = 100;
//	std::vector<std::pair<int, std::string>> items;
//	int alertness = 100;
//	int satiety = 100;
//	int socialScore = 100;
//	Schedule agentSchedule;
//};
