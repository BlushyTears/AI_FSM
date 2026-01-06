#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <queue>
#include <unordered_map>

#include "raylib.h"
#include "resource_dir.h"	

#include "FSMLibrary.h"

struct Agent;
// Metaphor for making agents speak with other 
// (Oh how I overcomplicated that process in my head before just doing this)
namespace Wifi {
	inline std::vector<Agent> agents;
}

struct Agent {
	int id;
	int money = 40;
	std::vector<std::pair<int, std::string>> items;
	int sleep = 79;
	int satiety = 60;
	int socialScore = 50;

	int metabolismRate;
	int costOfLiving;
	int drowsynessRate;
	int extroversionRate;
	Agent(int _metabolismRate, int _costOfLiving, int _drowsynessRate, int _extroVersionRate) 
	:	metabolismRate(_metabolismRate),
		costOfLiving(_costOfLiving),
		drowsynessRate(drowsynessRate),
		extroversionRate(_extroVersionRate)
	{

	}

	StateMachine<Agent> sm;

	bool operator==(const Agent& agent) const {
		return this->id == agent.id;
	}

	std::vector<Agent> getMatchingAgents(Agent& agent) {
		std::vector<Agent> potentialAgents;

		for (auto& externAgent : Wifi::agents) {
			if (externAgent.sm.currentState == agent.sm.currentState
				&& externAgent.id != agent.id) {
				potentialAgents.push_back(externAgent);
			}
		}

		return potentialAgents;
	}
};


// - Behavior trees -

// Agent Should pick job based on much money he has
// We arbitrarily pick one of the jobs by making agent spend enough money at once
template <typename T>
struct BrickLayingTask : Task<T> {
	int goalMoney = 70;
	int salary = 15;
	bool run(T& agent) override {
		std::cout << "Agent " << agent.id << " is doing laybricking, money: " << agent.money << std::endl;;
		agent.money += salary;
		if (agent.money >= goalMoney)
			return true;
		return false;
	}
};

template <typename T>
struct CarpentryLabourTask : Task<T> {
	int goalMoney = 40;
	int salary = 25;

	bool run(T& agent) override {
		std::cout << "Agent " << agent.id << " is doing carpentry, money: " << agent.money << std::endl;
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
		if (agent.money <= 20) {
			this->children.push_back(&blt);
		}
		else {
			this->children.push_back(&clt);
		}
	}
};

template <typename T>
struct Drink : Task<T> {
	bool run(T& agent) override {
		if (agent.satiety > 70)
			return true;

		std::vector<Agent> matchingAgents = agent.getMatchingAgents(agent);

		if (matchingAgents.size() == 0) {
			std::cout << "[Task] Agent " << agent.id << " is drinking some soda at the resturant, money: " << agent.money << std::endl;
		}
		else {
			for (auto& meetingAgent : matchingAgents) {
				agent.socialScore += 5;
				std::cout << "[Task] Agent " << agent.id << ", " 
						<< " Is drinking soda at some resturant (with Agent " << meetingAgent.id << "), money: " 
						<< agent.money << meetingAgent.id << std::endl;
			}
		}

		agent.satiety += 5;
	}
};

template <typename T>
struct Eat : Task<T> {
	bool run(T& agent) override {
		if (agent.satiety > 60)
			return true;
		std::cout << "[Task] Agent " << agent.id << " is eating a burger" << std::endl;
		agent.satiety += 10;
	}
};

template <typename T>
struct EatAndDrinkBehavior : Sequence<T> {
	Eat<T> eating;
	Drink<T> drinking;

	EatAndDrinkBehavior() {
		this->children.push_back(&eating);
		this->children.push_back(&drinking);
	}
};

// - Decision trees - 

// Actions
template <typename T>
struct SleepingAction : Action<T> {
	void execute(T& agent) override {
		std::cout << "[Action] Agent " << agent.id << " Is Sleeping.. " << std::endl;
		agent.sleep += 15;
	}
};

template <typename T>
struct EatingAction : Action<T> {
	EatAndDrinkBehavior<T>* eatAndDrink = nullptr;

	void execute(T& agent) override {
		if (eatAndDrink == nullptr) {
			eatAndDrink = new EatAndDrinkBehavior<T>();
		}
		std::cout << "[Action] agent " << agent.id << " is eating " << std::endl;
		eatAndDrink->run(agent);
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
		std::cout << "[Action] Agent " 
			<< agent.id << " Is going to work.. , money: "
			<< agent.money << std::endl;
		while(!cm->run(agent)) {}
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
		if (agent.satiety < 40 && agent.money > 10) {
			std::cout << "[Decision] Agent " << agent.id 
				<< " is hungry, switching to eating mode, hunger:" 
				<< agent.satiety  << std::endl;
			return this->trueNode;
		}
		return this->falseNode;
	}
};

template <typename T>
struct SleepingDecision : Decision<T> {
	T testValue(T& agent) override { return agent; }
	DecisionTreeNode<T>* getBranch(T& agent) override {
		if (agent.sleep < 30) {
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
