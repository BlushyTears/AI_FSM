
#include "raylib.h"
#include "FSMLibrary.h"
#include <vector>

#include "resource_dir.h"	
#include "AgentNew.h"
#include "Clock.h"

// External function to trickle down the agent's fields (Taxes, hunger naturally dissipating etc)
void externalAgentTrickle(Agent& agent) {
	agent.money -= 10;
	agent.satiety -= 10;
}

// Socializing component idea:
// We use a singleton for fetching other agents, 
// Each agent will just look at what a particular agent will do
// And if it matches their own, then console just says "Agent 1 went out to eat with Agent 2" and vice versa
// Not super efficient with O(N!), but it's just a simple check with limited agents so it should be fine

// Another thing is that we should have a timer of 10 seconds for each action, rather than using i in loop
const int MAXIMUM_AGENTS = 4;
// Worker struct 
int main ()
{
	SleepingState<Agent>* sleepingState = new SleepingState<Agent>();
	EatingState<Agent>* eatingState = new EatingState<Agent>();
	WorkingState<Agent>* workingState = new WorkingState<Agent>();

	TargetSleepingState<Agent>* targetSleeping = new TargetSleepingState<Agent>(sleepingState);
	TargetEatingState<Agent>* targetEating = new TargetEatingState<Agent>(eatingState);
	TargetWorkingState<Agent>* targetWorking = new TargetWorkingState<Agent>(workingState);

	SleepingDecision<Agent>* sleepingCheck = new SleepingDecision<Agent>();
	HungerDecision<Agent>* hungerCheck = new HungerDecision<Agent>();
	WorkingDecision<Agent>* workingCheck = new WorkingDecision<Agent>();

	DecisionTreeTransition<Agent>* toSleeping = new DecisionTreeTransition<Agent>();
	DecisionTreeTransition<Agent>* toEating = new DecisionTreeTransition<Agent>();
	DecisionTreeTransition<Agent>* toWorking = new DecisionTreeTransition<Agent>();

	sleepingCheck->trueNode = targetSleeping;
	sleepingCheck->falseNode = nullptr;
	hungerCheck->trueNode = targetEating;
	hungerCheck->falseNode = nullptr;
	workingCheck->trueNode = targetWorking;
	workingCheck->falseNode = nullptr;

	toSleeping->decisionTreeRoot = sleepingCheck;
	toEating->decisionTreeRoot = hungerCheck;
	toWorking->decisionTreeRoot = workingCheck;

	sleepingState->transitions.push_back(toEating);
	sleepingState->transitions.push_back(toWorking);
	eatingState->transitions.push_back(toSleeping);
	eatingState->transitions.push_back(toWorking);
	workingState->transitions.push_back(toSleeping);
	workingState->transitions.push_back(toEating);

	Agent bob(3, 3, 2, 5);
	bob.id = 1;
	bob.sm = StateMachine<Agent>(eatingState);

	Agent bob1(3, 2, 2, 1);
	bob1.id = 2;
	bob1.sm = StateMachine<Agent>(sleepingState);

	Agent bob2(4, 5, 1, 6);
	bob2.id = 3;
	bob2.sm = StateMachine<Agent>(workingState);

	Wifi::agents.push_back(bob);
	Wifi::agents.push_back(bob1);
	Wifi::agents.push_back(bob2);

	std::cout << "Initating..." << std::endl;
	std::vector<std::vector<Action<Agent>*>> plans;

	for (int i = 0; i < 25; i++) {
		if (Wifi::agents.size() == 0) {
			std::cout << "----------------------------------------------------" << std::endl;
			std::cout << "- Ending simulation early since all agents have died -" << std::endl;
			std::cout << "----------------------------------------------------" << std::endl;
			break;
		}
		for (auto& agent : Wifi::agents) {
			if (agent.satiety <= 0) {
				std::cout << "Agent << " << agent.id << " died, removing from list of agents!" << std::endl;
				auto it = find(Wifi::agents.begin(), Wifi::agents.end(), agent);
				Wifi::agents.erase(it);
				break;
			}
			externalAgentTrickle(agent);

			std::vector<Action<Agent>*> plan = agent.sm.update(agent);

			for (auto& action : plan) {
				action->execute(agent);
			}
		}
		std::cout << "---------------------------------" << std::endl;
	}

	return 0;
}
