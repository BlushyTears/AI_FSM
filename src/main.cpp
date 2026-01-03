
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
	SleepingState<Agent>* sleepingState = new SleepingState<Agent>();
	EatingState<Agent>* eatingState = new EatingState<Agent>();

	TargetEatingState<Agent>* targetEating = new TargetEatingState<Agent>(eatingState);
	HungerDecision<Agent>* hungerCheck = new HungerDecision<Agent>();

	hungerCheck->trueNode = targetEating;
	hungerCheck->falseNode = nullptr;

	TargetSleepingState<Agent>* targetSleeping = new TargetSleepingState<Agent>(sleepingState);
	SleepingDecision<Agent>* sleepingCheck = new SleepingDecision<Agent>();

	sleepingCheck->trueNode = targetSleeping;
	sleepingCheck->falseNode = nullptr;

	DecisionTreeTransition<Agent>* toEating = new DecisionTreeTransition<Agent>();
	DecisionTreeTransition<Agent>* toSleeping = new DecisionTreeTransition<Agent>();

	toEating->decisionTreeRoot = hungerCheck;
	toSleeping->decisionTreeRoot = sleepingCheck;

	sleepingState->transitions.push_back(toEating);
	eatingState->transitions.push_back(toSleeping);

	Agent bob;
	bob.id = 1;
	bob.sm = StateMachine<Agent>(sleepingState);

	std::cout << "Initating..." << std::endl;

	for (int i = 0; i < 25; i++) {
		std::vector<Action<Agent>*> plan = bob.sm.update(bob);

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