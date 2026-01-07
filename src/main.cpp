
#include "raylib.h"
#include "FSMLibrary.h"
#include <vector>

#include "resource_dir.h"	
#include "AgentNew.h"
#include "Clock.h"

// External function to trickle down the agent's fields (Taxes, digestion etc)
void externalAgentTrickle(Agent& agent) {
	agent.money -= agent.costOfLiving;
	agent.satiety -= agent.metabolismRate;
	agent.sleep -= agent.drowsynessRate;
	agent.socialScore -= agent.extroversionRate;
}

// How long the simulation will go on for. 
// Since the program doesn't sleep, 50 can mean anything
// But for instance it could represent hour since that's
// roughly what a task/action might take in real life.
constexpr int SIMULATION_TIME = 50;
int main ()
{
	SleepingState<Agent>* sleepingState = new SleepingState<Agent>();
	EatingState<Agent>* eatingState = new EatingState<Agent>();
	WorkingState<Agent>* workingState = new WorkingState<Agent>();
	SpendingState<Agent>* spendingState = new SpendingState<Agent>();
	SocializingState<Agent>* socializingState = new SocializingState<Agent>();

	TargetSleepingState<Agent>* targetSleeping = new TargetSleepingState<Agent>(sleepingState);
	TargetEatingState<Agent>* targetEating = new TargetEatingState<Agent>(eatingState);
	TargetWorkingState<Agent>* targetWorking = new TargetWorkingState<Agent>(workingState);
	TargetSpendingState<Agent>* targetSpending = new TargetSpendingState<Agent>(spendingState);
	TargetSocializingState<Agent>* targetSocializing = new TargetSocializingState<Agent>(socializingState);

	SleepingDecision<Agent>* sleepingCheck = new SleepingDecision<Agent>();
	HungerDecision<Agent>* hungerCheck = new HungerDecision<Agent>();
	WorkingDecision<Agent>* workingCheck = new WorkingDecision<Agent>();
	SpendingDecision<Agent>* spendingCheck = new SpendingDecision<Agent>();
	SocializingDecision<Agent>* socializingCheck = new SocializingDecision<Agent>();

	DecisionTreeTransition<Agent>* toSleeping = new DecisionTreeTransition<Agent>();
	DecisionTreeTransition<Agent>* toEating = new DecisionTreeTransition<Agent>();
	DecisionTreeTransition<Agent>* toWorking = new DecisionTreeTransition<Agent>();
	DecisionTreeTransition<Agent>* toSpending = new DecisionTreeTransition<Agent>();
	DecisionTreeTransition<Agent>* toSocializing = new DecisionTreeTransition<Agent>();

	sleepingCheck->trueNode = targetSleeping;
	sleepingCheck->falseNode = nullptr;
	hungerCheck->trueNode = targetEating;
	hungerCheck->falseNode = nullptr;
	workingCheck->trueNode = targetWorking;
	workingCheck->falseNode = nullptr;
	socializingCheck->trueNode = targetSocializing;
	socializingCheck->falseNode = nullptr;
	spendingCheck->trueNode = targetSpending;
	spendingCheck->falseNode = nullptr;

	toSocializing->decisionTreeRoot = socializingCheck;
	toSleeping->decisionTreeRoot = sleepingCheck;
	toEating->decisionTreeRoot = hungerCheck;
	toWorking->decisionTreeRoot = workingCheck;
	toSpending->decisionTreeRoot = spendingCheck;

	// Just to demonstrate the power of this FSM, you can go from socializing to the other states
	// Additionally, you can only go into 'spending state' if you are either currently working or eating

	sleepingState->transitions.push_back(toSocializing);
	sleepingState->transitions.push_back(toEating);
	sleepingState->transitions.push_back(toWorking);

	eatingState->transitions.push_back(toSleeping);
	eatingState->transitions.push_back(toWorking);
	eatingState->transitions.push_back(toSpending);

	workingState->transitions.push_back(toEating);
	workingState->transitions.push_back(toSocializing);
	workingState->transitions.push_back(toSleeping);
	workingState->transitions.push_back(toSpending);

	spendingState->transitions.push_back(toSleeping);
	spendingState->transitions.push_back(toEating);
	spendingState->transitions.push_back(toWorking);

	socializingState->transitions.push_back(toSleeping);
	socializingState->transitions.push_back(toSpending);

	Agent bob1(1, 3, 2, 2, 150, 90);
	bob1.id = 1;
	bob1.sm = StateMachine<Agent>(eatingState);

	Agent bob2(4, 2, 7, 3, 70, 190);
	bob2.id = 2;
	bob2.sm = StateMachine<Agent>(sleepingState);

	Agent bob3(3, 5, 2, 4, 50, 50);
	bob3.id = 3;
	bob3.sm = StateMachine<Agent>(workingState);

	Agent bob4(12, 4, 5, 4, 20, 60);
	bob4.id = 4;
	bob4.sm = StateMachine<Agent>(spendingState);

	Wifi::agents.push_back(bob1);
	Wifi::agents.push_back(bob2);
	Wifi::agents.push_back(bob3);
	Wifi::agents.push_back(bob4);

	std::cout << "Initating..." << std::endl << "----------------------------" << std::endl;
	std::vector<std::vector<Action<Agent>*>> plans;

	// Various agents have different starting states to make it slightly more interesting
	for (int i = 0; i < SIMULATION_TIME; i++) {
		if (Wifi::agents.size() == 0) {
			std::cout << "----------------------------------------------------" << std::endl;
			std::cout << "- Ending simulation early since all agents have died -" << std::endl;
			std::cout << "----------------------------------------------------" << std::endl;
			break;
		}
		for (auto& agent : Wifi::agents) {
			if (agent.satiety <= 0 || agent.sleep <= 0) {
				std::cout << "[DEATH] Agent " 
							<< agent.id 
							<< " died from either lack of hunger or lack of sleep," << std::endl
							<< " Hunger: " << agent.satiety << " Sleep" << agent.sleep
							<< ". Removing this from list of agents!" 
							<< std::endl;
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
		std::cout << "----------------------------------------------------------------------------------" << std::endl;
	}

	std::cout << "Simulation has ended, " << Wifi::agents.size() << " Agents survived" << std::endl;
	return 0;
}

// Socializing component idea notes:
// We use a singleton for fetching other agents, 
// Each agent will just look at what a particular agent will do
// And if it matches their own, then console just says "Agent 1 went out to eat with Agent 2" and vice versa
// Not super efficient with O(N!), but it's just a simple check with limited agents so it should be fine
// Plus it's a super concise (and quite elegant) solution code wise!

