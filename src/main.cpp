
#include "raylib.h"
#include "FSMLibrary.h"
#include <vector>

#include "resource_dir.h"	
#include "AgentNew.h"
#include "Clock.h"

// External function to trickle down the agent's fields (Taxes, hunger naturally dissipating etc)
void externalAgentTrickle(Agent& agent) {
	agent.money -= 10;
	agent.satiety -= 3;
}

const int MAXIMUM_AGENTS = 4;
// Worker struct 
int main ()
{
	SleepingState<Agent>* sleepingState = new SleepingState<Agent>();
	EatingState<Agent>* eatingState = new EatingState<Agent>();
	WorkingState<Agent>* workingState = new WorkingState<Agent>();

	TargetEatingState<Agent>* targetEating = new TargetEatingState<Agent>(eatingState);
	TargetWorkingState<Agent>* targetWorking = new TargetWorkingState<Agent>(workingState);
	HungerDecision<Agent>* hungerCheck = new HungerDecision<Agent>();
	WorkingDecision<Agent>* workingCheck = new WorkingDecision<Agent>();

	hungerCheck->trueNode = targetEating;
	hungerCheck->falseNode = nullptr;

	workingCheck->trueNode = targetWorking;
	workingCheck->falseNode = nullptr;

	TargetSleepingState<Agent>* targetSleeping = new TargetSleepingState<Agent>(sleepingState);
	SleepingDecision<Agent>* sleepingCheck = new SleepingDecision<Agent>();

	sleepingCheck->trueNode = targetSleeping;
	sleepingCheck->falseNode = nullptr;

	DecisionTreeTransition<Agent>* toEating = new DecisionTreeTransition<Agent>();
	DecisionTreeTransition<Agent>* toSleeping = new DecisionTreeTransition<Agent>();
	DecisionTreeTransition<Agent>* toWorking = new DecisionTreeTransition<Agent>();

	toEating->decisionTreeRoot = hungerCheck;
	toSleeping->decisionTreeRoot = sleepingCheck;
	toWorking->decisionTreeRoot = workingCheck;

	sleepingState->transitions.push_back(toEating);
	sleepingState->transitions.push_back(toWorking);

	eatingState->transitions.push_back(toSleeping);
	eatingState->transitions.push_back(toWorking);

	workingState->transitions.push_back(toSleeping);
	workingState->transitions.push_back(toEating);

	Agent bob(2, 25);
	bob.id = 1;
	bob.sm = StateMachine<Agent>(eatingState);

	Agent bob1(22, 5);
	bob1.id = 2;
	bob1.sm = StateMachine<Agent>(sleepingState);

	std::vector<Agent> agents;

	agents.push_back(bob);
	agents.push_back(bob1);

	std::cout << "Initating..." << std::endl;

	std::vector<std::vector<Action<Agent>*>> plans;

	for (int i = 0; i < 25; i++) {
		for (auto& agent : agents) {
			externalAgentTrickle(agent);

			std::vector<Action<Agent>*> plan = agent.sm.update(agent);

			for (auto* action : plan) {
				action->execute(agent);
			}
		}
		std::cout << "---------------------------------" << std::endl;

	}

	//for (auto& agent : agents) {
	//	for (auto& plan : plans) {
	//		for (auto* action : plan) {
	//			action->execute(agent);
	//			std::cout << "Hunger: " << agent.satiety;
	//		}
	//	}
	//	std::cout << "---------------------------------" << std::endl;
	//}

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