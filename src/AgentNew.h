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
struct Agent {
	int id;
	int money = 100;
	std::vector<std::pair<int, std::string>> items;
	int alertness = 100;
	int satiety = 100;
	int socialScore = 100;
	StateMachine<Agent<T>> sm;
};

// Actions
template <typename T>
struct IdleAction : Action<T> {
	void execute(T& agent) override {
		std::cout << "Agent" << agent.id << " Is Sleeping.. " << std::endl;
		agent.satiety -= 10;
	}
};

template <typename T>
struct EatingAction : Action<T> {
	void execute(T& agent) override {
		std::cout << "Agent" << agent.id << " Is eating.. " << std::endl;
		agent.satiety += 80;
	}
};

// States
template <typename T>
struct SleepingState : State<T> {
	IdleAction<T> sleeping;
	std::vector<Transition<T>*> transitions;

	std::vector<Action<T>*> getActions() override { return { &sleeping }; };
	std::vector<Transition<T>*> getTransitions() override { return transitions; };
};

// 
template <typename T>
struct EatingState : State<T> {
	EatingAction<T> eating;
	std::vector<Transition<T>*> transitions;

	std::vector<Action<T>*> getActions() override { return { &eating }; };
	std::vector<Transition<T>*> getTransitions() override { return transitions; };
};

// Target states
template<typename T>
struct TargetEatingState : TargetState<T> {
	EatingState<T>* eatingState;
	TargetEatingState(EatingState<T>* s) : eatingState(s) {}

	std::vector<Action<T>*> getActions() override { return {}; }
	State<T>* getTargetState() override { return eatingState; }
};

template <typename T>
struct TargetSleepingState : TargetState<T> {
	SleepingState<T>* sleepingState;
	TargetSleepingState(SleepingState<T>* s) : sleepingState(s) {}

	std::vector<Action<T>*> getActions() override { return {}; }
	State<T>* getTargetState() override { return sleepingState; }
};

// Decision tree nodes
template <typename T>
struct HungerDecision : Decision<T> {
	T testValue(T& agent) override { return agent; }

	DecisionTreeNode<T>* getBranch(T& agent) override {
		if (agent.satiety < 20) {
			std::cout << "[Decision] Agent is hungry, switching to eating mode" << std::endl;
			return this->trueNode;
		}
		return this->falseNode;
	}
};

template <typename T>
struct SleepingDecision : Decision<T> {
	T testValue(T& agent) override { return agent; }

	DecisionTreeNode<T>* getBranch(T& agent) override {
		if (agent.satiety > 50) {
			std::cout << "[Decision] Agent is tired, switching to sleeping mode" << std::endl;
			return this->trueNode;
		}
		return this->falseNode;
	}
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
