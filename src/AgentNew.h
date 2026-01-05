#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <queue>
#include <unordered_map>

#include "raylib.h"
#include "resource_dir.h"	

#include "FSMLibrary.h"

struct Agent {
	int id;
	int money = 100;
	std::vector<std::pair<int, std::string>> items;
	int alertness = 100;
	int satiety = 100;
	int socialScore = 100;

	int satietyGain;
	int satietyLoss;

	Agent(int _satietyLoss, int _satietyGain) {
		satietyLoss = _satietyLoss;
		satietyGain = _satietyGain;
	}

	StateMachine<Agent> sm;
};

// - Behavior trees -

// Agent Should pick job based on much money he has
// We artificially enforce one of the jobs by making agent spend enough money at once

template <typename T>
struct BrickLayingTask : Task<T> {
	int goalMoney = 90;
	int salary = 15;
	bool run(T& agent) override {
		agent.money += salary;
		if (agent.money >= goalMoney)
			return true;
		return false;
	}
};

template <typename T>
struct CarpentryLabourTask : Task<T> {
	int goalMoney = 50;
	int salary = 5;

	bool run(T& agent) override {
		agent.money += salary;
		if (agent.money >= goalMoney)
			return true;
		return false;
	}
};

template <typename T>
struct CollectMoney : Selector<T> {
	BrickLayingTask<T> blt;
	CarpentryLabourTask<T> clt;
	// Pick one of two jobs depending on how much money the agent has
	CollectMoney(T& agent) {
		if (agent.money > 40) {
			this->children.push_back(&blt);
			this->children.push_back(&clt);
		}
		else {
			this->children.push_back(&clt);
			this->children.push_back(&blt);
		}
	}
};

// - Decision trees - 

// Actions
template <typename T>
struct SleepingAction : Action<T> {
	void execute(T& agent) override {
		std::cout << "Agent " << agent.id << " Is Sleeping.. " << std::endl;
		agent.satiety -= agent.satietyLoss;
	}
};

template <typename T>
struct EatingAction : Action<T> {
	void execute(T& agent) override {
		std::cout << "Agent " << agent.id << " Is eating.. " << std::endl;
		agent.satiety += agent.satietyGain;
	}
};

// Kind of ugly, but we need a reference to agent for working behavior which is parsed to collect money
template <typename T>
struct WorkingAction : Action<T> {
	CollectMoney<T>* cm = nullptr;

	~WorkingAction() {
		if (cm != nullptr) {
			delete cm;
		}
	}

	void execute(T& agent) override {
		if (cm == nullptr) {
			// here we parse the agent
			cm = new CollectMoney<T>(agent);
		}
		while(!cm->run(agent)) {
			std::cout << "Agent " << agent.id << " Is working.. " << std::endl;
		}
	}
};

// States
template <typename T>
struct SleepingState : State<T> {
	SleepingAction<T> sleeping;
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

template <typename T>
struct WorkingState : State<T> {
	WorkingAction<T> working;
	std::vector<Transition<T>*> transitions;

	std::vector<Action<T>*> getActions() override { return { &working }; };
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

template<typename T>
struct TargetWorkingState : TargetState<T> {
	WorkingState<T>* workingState;

	TargetWorkingState(WorkingState<T>* s) : workingState(s) {}

	std::vector<Action<T>*> getActions() override { return {}; }
	State<T>* getTargetState() override { return workingState; }
};

// Decision tree nodes
template <typename T>
struct HungerDecision : Decision<T> {
	T testValue(T& agent) override { return agent; }
	DecisionTreeNode<T>* getBranch(T& agent) override {
		if (agent.satiety < 60 && agent.money > 30) {
			std::cout << "[Decision] Agent " << agent.id << " is hungry, switching to eating mode" << std::endl;
			return this->trueNode;
		}
		return this->falseNode;
	}
};

template <typename T>
struct SleepingDecision : Decision<T> {
	T testValue(T& agent) override { return agent; }
	DecisionTreeNode<T>* getBranch(T& agent) override {
		if (agent.satiety > 60) {
			std::cout << "[Decision] Agent " << agent.id << "  is tired, switching to sleeping mode" << std::endl;
			return this->trueNode;
		}
		return this->falseNode;
	}
};

template <typename T>
struct WorkingDecision : Decision<T> {
	T testValue(T& agent) override { return agent; }
	DecisionTreeNode<T>* getBranch(T& agent) override {
		if (agent.money < 20 && agent.satiety > 10) {
			std::cout << "[Decision] Agent " << agent.id << "  needs money, switching to working mode" << std::endl;
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
