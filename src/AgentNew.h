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
	int items = 0;
	int sleep = 79;
	int satiety = 60;
	int socialScore = 50;

	int metabolismRate;
	int costOfLiving;
	int drowsynessRate;
	int extroversionRate;
	Agent(int _metabolismRate, int _costOfLiving, 
			int _drowsynessRate, int _extroVersionRate,
			int _startingMoney, int _startingHunger) 
	:	metabolismRate(_metabolismRate),
		costOfLiving(_costOfLiving),
		drowsynessRate(_drowsynessRate),
		extroversionRate(_extroVersionRate),
		money(_startingMoney),
		satiety(_startingHunger)
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

// Agent picks job based on much money it has
template <typename T>
struct BrickLayingTask : Task<T> {
	int goalMoney = 50;
	int salary = 35;
	bool run(T& agent) override {
		if (agent.money >= goalMoney)
			return false;
		agent.money += salary;
		std::cout << "[Task] Agent " << agent.id 
					<< " is doing laybricking, money: " 
					<< agent.money 
					<< std::endl;;
		return true;
	}
};

template <typename T>
struct CarpentryLabourTask : Task<T> {
	int goalMoney = 150;
	int salary = 15;

	bool run(T& agent) override {
		if (agent.money >= goalMoney)
			return false;
		agent.money += salary;
		std::cout << "[Task] Agent " 
					<< agent.id 
					<< " is doing carpentry, money: " 
					<< agent.money 
					<< std::endl;
		return true;
	}
};

template <typename T>
struct CollectMoney : Selector<T> {
	BrickLayingTask<T> blt;
	CarpentryLabourTask<T> clt;
	CollectMoney(T& agent) {
		this->children.push_back(&blt);
		this->children.push_back(&clt);
	}
};

template <typename T>
struct Drink : Task<T> {
	bool run(T& agent) override {
		if (agent.satiety > 40)
			return true;

		std::vector<Agent> matchingAgents = agent.getMatchingAgents(agent);

		if (matchingAgents.size() == 0) {
			std::cout << "[Task] Agent " << agent.id 
						<< " is alone drinking some soda at the resturant, money: "
						<< agent.satiety << ", social score: " << agent.socialScore << std::endl;
		}
		else {
			for (auto& meetingAgent : matchingAgents) {
				agent.socialScore += 5;
				std::cout << "[Task] Agent " << agent.id << ", " 
							<< " Is drinking soda at some resturant (with Agent " 
							<< meetingAgent.id << "), money: " 
							<< agent.money << meetingAgent.id 
							<< " social score: " 
							<< agent.socialScore 
							<< std::endl;
			}
		}
		agent.satiety += 5;
	}
};

template <typename T>
struct EatBurger : Task<T> {
	bool run(T& agent) override {
		if (agent.satiety > 60)
			return true;

		std::vector<Agent> matchingAgents = agent.getMatchingAgents(agent);

		if (matchingAgents.size() == 0) {
			std::cout << "[Task] Agent " << agent.id
						<< " is alone eating a burger at the resturant. Money: " 
						<< agent.money << ", SocialScore: "
						<< agent.socialScore << std::endl;
		}
		else {
			for (auto& meetingAgent : matchingAgents) {
				agent.socialScore += 5;
				std::cout << "[Task] Agent " << agent.id << ", "
							<< " Is eating burgers at some resturant (with Agent "
							<< meetingAgent.id << "). Money: "
							<< agent.money << meetingAgent.id << std::endl;
			}
		}
		agent.satiety += 10;
	}
};

template <typename T>
struct EatAndDrinkBehavior : Sequence<T> {
	EatBurger<T> eating;
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
		std::cout << "[Action] Agent " << agent.id << " Is Sleeping. Sleep: " << agent.sleep << std::endl;
		agent.sleep += 10;
	}
};

template <typename T>
struct EatingAction : Action<T> {
	EatAndDrinkBehavior<T>* eatAndDrink = nullptr;
	void execute(T& agent) override {
		if (eatAndDrink == nullptr) {
			eatAndDrink = new EatAndDrinkBehavior<T>();
		}
		std::cout << "[Action] agent " << agent.id << " is eating. Hunger: " << agent.satiety << std::endl;
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
 		cm->run(agent);
	}
};

template <typename T>
struct SpendingAction : Action<T> {
	int shovelPrice = 15;
	int bootsPrice = 25;
	int hatPrice = 50;
	void execute(T& agent) override {
		std::string item;
		if (agent.money > hatPrice) {
			item = "A Hat!";
			agent.money -= hatPrice;
		}
		else if (agent.money <= hatPrice && agent.money > bootsPrice) {
			item = "Boots!";
			agent.money -= bootsPrice;
		}
		else {
			item = "A Shovel!";
			agent.money -= shovelPrice;
		}
		agent.items++;
		std::cout << "[Action] Agent "
					<< agent.id << " went to to the store and bought " 
					<< item << " Total Items bought: " 
					<< agent.items
					<< item << ". Money: "
					<< agent.money 
					<< std::endl;
	}
};

template <typename T>
struct SocializingAction : Action<T> {
	void execute(T& agent) override {
		std::vector<Agent> matchingAgents = agent.getMatchingAgents(agent);

		if (matchingAgents.size() == 0) {
			std::cout << "[Task] Agent " << agent.id
				<< " Had noboy to go our with so it stayed home :(. SocialScore: "
				<< agent.socialScore << ", SocialScore: " << std::endl;
			agent.socialScore -= 2; // Agent felt extra sad now :(
		}
		else {
			std::cout << "[Action] Agent " << agent.id << " Is going out with Agent: ";
			for (auto& meetingAgent : matchingAgents) {
				std::cout << meetingAgent.id << ", ";
				agent.socialScore += 25; // happy!
			}
			std::cout << std::endl;
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

template <typename T>
struct SpendingState : State<T> {
	SpendingAction<T> spending;
	std::vector<Transition<T>*> transitions;

	std::vector<Action<T>*> getActions() override { return { &spending }; };
	std::vector<Transition<T>*> getTransitions() override { return transitions; };
};

template <typename T>
struct SocializingState : State<T> {
	SocializingAction<T> socializing;
	std::vector<Transition<T>*> transitions;

	std::vector<Action<T>*> getActions() override { return { &socializing }; };
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

template<typename T>
struct TargetSpendingState : TargetState<T> {
	SpendingState<T>* spendingState;

	TargetSpendingState(SpendingState<T>* s) : spendingState(s) {}

	std::vector<Action<T>*> getActions() override { return {}; }
	State<T>* getTargetState() override { return spendingState; }
};

template<typename T>
struct TargetSocializingState : TargetState<T> {
	SocializingState<T>* socializingState;

	TargetSocializingState(SocializingState<T>* s) : socializingState(s) {}

	std::vector<Action<T>*> getActions() override { return {}; }
	State<T>* getTargetState() override { return socializingState; }
};

// Decision tree nodes
template <typename T>
struct HungerDecision : Decision<T> {
	T testValue(T& agent) override { return agent; }
	DecisionTreeNode<T>* getBranch(T& agent) override {
		if (agent.satiety < 40) {
			std::cout << "[Decision] Agent " << agent.id 
						<< " is hungry, switching to eating mode. Hunger: " 
						<< agent.satiety << ", money: " << agent.money << std::endl;
			return this->trueNode;
		}
		return this->falseNode;
	}
};

template <typename T>
struct SleepingDecision : Decision<T> {
	T testValue(T& agent) override { return agent; }
	DecisionTreeNode<T>* getBranch(T& agent) override {
		if (agent.sleep < 30 && agent.satiety > 20) {
			std::cout << "[Decision] Agent " << agent.id 
						<< "  is tired, switching to sleeping mode. Sleep Score: " 
						<< agent.sleep << std::endl;
			return this->trueNode;
		}
		return this->falseNode;
	}
};

template <typename T>
struct WorkingDecision : Decision<T> {
	T testValue(T& agent) override { return agent; }
	DecisionTreeNode<T>* getBranch(T& agent) override {
		if (agent.money < 10 && agent.satiety >= 10 && agent.sleep > 30) {
			std::cout << "[Decision] Agent " 
						<< agent.id 
						<< " needs money, switching to working mode. Money:" 
						<< agent.money 
						<< std::endl;
			return this->trueNode;
		}
		return this->falseNode;
	}
};

template <typename T>
struct SpendingDecision : Decision<T> {
	T testValue(T& agent) override { return agent; }
	DecisionTreeNode<T>* getBranch(T& agent) override {
		// we really wanna reduce the spending hehe
		if (agent.money > 100 && agent.satiety > 30 && agent.sleep > 30) {
			std::cout << "[Decision] Agent " << agent.id 
						<< "  is feeling rich, switching to spending mode. Money:"
						<< agent.money
						<< std::endl;
			return this->trueNode;
		}
		return this->falseNode;
	}
};

template <typename T>
struct SocializingDecision : Decision<T> {
	T testValue(T& agent) override { return agent; }
	DecisionTreeNode<T>* getBranch(T& agent) override {
		// Here we explicitly tell the agents that we should only go out if we're not about to starve
		if (agent.socialScore < 10) {
			std::cout << "[Decision] Agent " 
						<< agent.id 
						<< "  is feeling lonely, switching to socializing mode" 
						<< std::endl;
			return this->trueNode;
		}
		return this->falseNode;
	}
};