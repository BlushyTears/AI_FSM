
#include "raylib.h"
#include "FSMLibrary.h"
#include <vector>

#include "resource_dir.h"	
#include "AgentNew.h"
#include "Clock.h"

// Actions
template <typename T>
struct IdleAction : Action<T> {
	void execute(T& agent) override {
		std::cout << "Agent" << agent.id << " Is idleing.. " << std::endl;
		agent.satiety -= 15;
	}
};

template <typename T>
struct EatingAction : Action<T> {
	void execute(T& agent) override {
		std::cout << "Agent" << agent.id << " Is eating.. " << std::endl;
		agent.satiety += 30;
	}
};

// States
template <typename T>
struct IdleState : State<T> {
	IdleAction<T> idle;
	std::vector<Transition<T>*> transitions;

	std::vector<Action<T>*> getActions() override { return { &idle }; };
	std::vector<Transition<T>*> getTransitions() override { return transitions; };
};

template <typename T>
struct EatingState : State<T> {
	EatingAction<T> eating;
	std::vector<Action<T>*> getActions() override { return { &eating }; };
};

// Decision tree nodes

template<typename T>
struct TargetEatingState : TargetState<T> {
	EatingState<T>* eatingState;
	TargetEatingState(EatingState<T>* s) : eatingState(s) {}

	std::vector<Action<T>*> getActions() override { return {}; }
	State<T>* getTargetState() override { return eatingState; }
};

template <typename T>
struct HungerDecision : Decision<T> {
	T testValue(T& agent) override { return agent; }

	DecisionTreeNode<T>* getBranch(T& agent) override {
		if (agent.satiety < 50) {
			std::cout << "[Decision] Agent is hungry, switching to eating mode" << std::endl;
			return this->trueNode;
		}
		return this->falseNode;
	}
};

const int MAXIMUM_AGENTS = 4;
// Worker struct 
int main ()
{
	IdleState<Agent<int>>* idleState = new IdleState<Agent<int>>();
	EatingState<Agent<int>>* eatingState = new EatingState<Agent<int>>();

	TargetEatingState<Agent<int>>* targetEating = new TargetEatingState<Agent<int>>(eatingState);
	HungerDecision<Agent<int>>* hungerCheck = new HungerDecision<Agent<int>>();

	hungerCheck->trueNode = targetEating;
	hungerCheck->falseNode = nullptr;

	DecisionTreeTransition<Agent<int>>* toEating = new DecisionTreeTransition<Agent<int>>();
	toEating->decisionTreeRoot = hungerCheck;

	idleState->transitions.push_back(toEating);

	Agent<int>bob;
	bob.id = 1;
	bob.satiety = 100;
	bob.sm = StateMachine<Agent<int>>(idleState);

	std::cout << "Initating..." << std::endl;

	for (int i = 0; i < 10; i++) {
		std::vector<Action<Agent<int>>*> plan = bob.sm.update(bob);

		for (auto* action : plan) {
			action->execute(bob);
		}

		std::cout << "---------------------------------" << std::endl;
	}

	return 0;
}

/*
	Simplest possible explanation of FSM + Decision Trees:

	A state is just the struct with transitions to other
	states or possible actions to perform

	A transition is just a struct with a boolean if it's triggered,
	and a target state that it points to.

	Target state is a special state which is created when a transition is true

	a decision tree node is a parrent class for either an action or decision

	A decision inherits from decision tree node and procures two leaf nodes

	An action inherits from decision tree node that implements the execute() function

	- STORAGE -
	An action generally stored in a list and
	is used by the current state's actions,
	the triggered transitions actions and
	the target state's actions.
*/