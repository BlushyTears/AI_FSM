
#include "raylib.h"
#include "FSMLibrary.h"
#include <vector>

#include "resource_dir.h"	
#include "AgentNew.h"
#include "Clock.h"

const int MAXIMUM_AGENTS = 4;
// Worker struct 
int main ()
{
	SleepingState<Agent<int>>* sleepingState = new SleepingState<Agent<int>>();
	EatingState<Agent<int>>* eatingState = new EatingState<Agent<int>>();

	TargetEatingState<Agent<int>>* targetEating = new TargetEatingState<Agent<int>>(eatingState);
	HungerDecision<Agent<int>>* hungerCheck = new HungerDecision<Agent<int>>();

	hungerCheck->trueNode = targetEating;
	hungerCheck->falseNode = nullptr;

	TargetSleepingState<Agent<int>>* targetSleeping = new TargetSleepingState<Agent<int>>(sleepingState);
	SleepingDecision<Agent<int>>* sleepingCheck = new SleepingDecision<Agent<int>>();

	sleepingCheck->trueNode = targetSleeping;
	sleepingCheck->falseNode = nullptr;

	DecisionTreeTransition<Agent<int>>* toEating = new DecisionTreeTransition<Agent<int>>();
	DecisionTreeTransition<Agent<int>>* toSleeping = new DecisionTreeTransition<Agent<int>>();

	toEating->decisionTreeRoot = hungerCheck;
	toSleeping->decisionTreeRoot = sleepingCheck;

	sleepingState->transitions.push_back(toEating);
	eatingState->transitions.push_back(toSleeping);

	Agent<int>bob;
	bob.id = 1;
	bob.sm = StateMachine<Agent<int>>(sleepingState);

	std::cout << "Initating..." << std::endl;

	for (int i = 0; i < 25; i++) {
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