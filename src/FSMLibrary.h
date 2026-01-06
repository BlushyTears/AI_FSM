#pragma once
#include <iostream>
#include <vector>
#include <queue>

// Design thoughts:
// When an agent is performing a particular activity, 
// he should print that state to screen, replenish that stat.
// Replenishing a stat although not specified as a requirement; 
// Should probably be at the expense of bleeding other stats

// Naturally it is expected the agent will die, and our goal is to 
// Make the most out of the limited resources we'll have access to 
// Before running out of them, most likely by utilizing a priority queue
// We know that starving is the only way for the agent to die, 
// so eating should be able to override all other items.

// Additionally, the messaging between agents & scheduling adds 
// Complexity since it's expected to adhere to both 
// decision trees & behavior trees indiscriminate of its underlying implementation.

// Implementation details:
// A particular behavior is just 3 things: print to console, refill stat and potentially message other agents
// Messaging is just: scheduling non-work related activities 
//					in order to max out the social score alongside the activity.
//					It should happen like a handshake: Agent 1 messages the other 3 agents
//					If they are available by offering his available hours assuming he isn't hungry
//					The other 3 agents will first also check their hunger levels and look at their
//					Available hours if they have other things planned, and if not, then meet at:
//					Either the bar, diner or store
// The messaging function should work as following: A public board (Metaphore is still telephone) 
// Where each agent can write their available hours and other agents can choose to join or write their own hours
// For instance if agent A writes 16, 17, 18; agent B is busy those hours, and therefore he writes 19
// And since agent C and D are both available at those hours, then they will join
// They could also write their own hours on top of joining, but my strategy to make them survive for longer
// Will be to minimize social hours, and therefore do something else for those hours.

/*
	Behavior trees small notes:

	Difference between behavior trees and state machines is that there's a single common interface
	for all tasks, which means they can be combined together without needing to know what else is in 
	the behavior tree


	Conditions check some property of the game such as proximity, line of sight
	Actions alter the state of the game
	Composite tasks

	Conditions and actions sit at the leaf of the tree 
*/

/* -FSM + Behavior trees - */

// Base class
template <typename T>
struct Task {
	virtual bool run(T& agent) = 0;
};

// Picks first possible child task that runs and returns true
template <typename T>
struct Selector : Task<T> {
	std::vector<Task<T>*> children;

	bool run(T& agent) {
		for (auto& c : children) {
			if (c->run(agent)) {
				return true;
			}
		}
		return false;
	}
};

// Only returns true if all children tasks ran
template <typename T>
struct Sequence : Task<T> {
	std::vector<Task<T>*> children;

	bool run(T& agent) {
		for (auto& c : children) {
			if (!c->run(agent)) {
				return false;
			}
		}
		return true;
	}
};

/* - FSM + Decision trees - */
template <typename T>
struct Transition;

// Interface used by either decision or action
template <typename T>
struct DecisionTreeNode {
	virtual DecisionTreeNode* makeDecision(T& agent) = 0;
};

// Idea: cast it during run-time if it's an action to access its fields
template <typename T>
struct Action : DecisionTreeNode<T> {
	DecisionTreeNode<T>* makeDecision(T& agent) override {
		return this;
	}

	virtual void execute(T& agent) = 0;
};

// Mostly an interface
template <typename T>
struct Decision : DecisionTreeNode<T> {
	DecisionTreeNode<T>* trueNode = nullptr;
	DecisionTreeNode<T>* falseNode = nullptr;

	// Should probably remove this even if book uses it
	virtual T testValue(T& agent) = 0;
	virtual DecisionTreeNode<T>* getBranch(T& agent) = 0;

	DecisionTreeNode* makeDecision(T& agent) override {
		DecisionTreeNode<T>* branch = getBranch(agent);
		if (branch == nullptr)
			return nullptr;
		return branch->makeDecision(agent);
	}
};

// Interface
template <typename T>
struct State {
	// Only get actions need to be enforced since each state has to do something
	virtual std::vector<Action<T>*> getActions() = 0;

	virtual std::vector<Action<T>*> getEntryActions() { return {}; };
	virtual std::vector<Action<T>*> getExitActions() { return {}; };
	virtual std::vector<Transition<T>*> getTransitions() { return {}; };
};

// Interface
template <typename T>
struct Transition {
	virtual bool isTriggered(T& agent) = 0;
	virtual State<T>* getTargetState() = 0;
	virtual	std::vector<Action<T>*> getActions() = 0;
};

template <typename T>
struct TargetState : DecisionTreeNode<T> {
	virtual std::vector<Action<T>*> getActions() = 0;
	virtual State<T>* getTargetState() = 0;

	// since target state is the end of a decision tree we should just return this
	DecisionTreeNode<T>* makeDecision(T& agent) override {
		return this;
	}
};

template <typename T>
struct DecisionTreeTransition : Transition<T> {
	TargetState<T>* targetState = nullptr;
	DecisionTreeNode<T>* decisionTreeRoot = nullptr;

	std::vector<Action<T>*> getActions() {
		if (targetState != nullptr)
			return targetState->getActions();
		return{};
	}

	State<T>* getTargetState() override {
		if (targetState != nullptr) {
			return targetState->getTargetState();
		}
		return nullptr;
	}

	bool isTriggered(T& agent) {
		targetState = static_cast<TargetState<T>*>(decisionTreeRoot->makeDecision(agent));
		return targetState != nullptr;
	}
};	

template <typename T>
struct StateMachine {
	State<T>* initialState = nullptr;
	State<T>* currentState = nullptr;
	TargetState<T>* targetState = nullptr;
	std::vector<Action<T>*> actions;

	StateMachine() = default;
	StateMachine(State<T>* startingState) : initialState(startingState), currentState(startingState) {}

	// Should be able to accept any type (Why we use templates)
	// of agent that implements the interface
	std::vector<Action<T>*> update(T& agent) {
		DecisionTreeTransition<T>* triggered = nullptr;

		for (auto* transition : currentState->getTransitions()) {
			if (transition->isTriggered(agent)) {
				triggered = static_cast<DecisionTreeTransition<T>*>(transition);
				break;
			}
		}
		if (triggered != nullptr) {
			actions = currentState->getExitActions();

			for (auto* a: triggered->getActions()) {
				actions.push_back(a);
			}

			State<T>* nextState = triggered->getTargetState();

			for (auto* a : nextState->getEntryActions()) {
				actions.push_back(a);
			}

			currentState = nextState;
			return actions;
		}
		else {
			return currentState->getActions();
		}
	}
};

