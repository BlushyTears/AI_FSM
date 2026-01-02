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

// Enum is probably a bad idea so i should not use this
enum Behaviors {
	Working,
	Buying,
	Sleeping,
	Eating,
	Socializing
};

template <typename T>
struct Node {
	T data;

	Node(T value) : data(value), trueNode(nullptr), falseNode(nullptr) {}
};

// Interface used by either decision or action
template <typename T>
struct DecisionTreeNode {
	virtual DecisionTreeNode makeDecision(Node<T> node) = 0;
};

// Idea: cast it during run-time if it's an action to access its fields
template <typename T>
struct Action : DecisionTreeNode {
	DecisionTreeNode makeDecision() {
		return this;
	}

	virtual void execute() = 0;
};

// Mostly an interface
template <typename T>
struct Decision : DecisionTreeNode {
	DecisionTreeNode trueNode;
	DecisionTreeNode falseNode;

	virtual T testValue() = 0;
	virtual DecisionTreeNode getBranch();

	DecisionTreeNode makeDecision() {
		DecisionTreeNode branch = getBranch();
		return branch.makeDecision();
	}
};

// Interface
template <typename T>
struct State {
	virtual std::vector<Action> getActions() = 0;
	virtual std::vector<Action> getEntryActions() = 0;
	virtual std::vector<Action> getExitActions() = 0;
	virtual std::vector<Transition> getTransitions() = 0;
};

// Interface
template <typename T>
struct Transition {
	virtual bool isTriggered() = 0;
	virtual State getTargetState() = 0;
	virtual	std::vector<Action> getActions() = 0;
};

template <typename T>
struct TargetState : DecisionTreeNode {
	virtual std::vector<Action> getActions() = 0;
	virtual State getTargetState() = 0;

	DecisionTreeNode makeDecision(DecisionTreeNode* node) {
		if (node == nullptr || dynamic_cast<TargetState*> node != nullptr) {
			return node;
		}
		else {
			if (node.test()) {
				return makeDecision(node.trueNode);
			}
			else {
				return makeDecision(node.falseNode);
			}
		}
	}
};

template <typename T>
struct DecisionTreeTransition : Transition {
	TargetState* targetState = nullptr;
	DecisionTreeNode* decisionTreeRoot = nullptr;

	std::vector<Action> getActions() {
		if (targetState != nullptr)
			return targetState->getActions();
		else
			return[];
	}

	State getTargetState() {
		if (targetState != nullptr) {
			targetState.getTargetState();
		}
		else
			return nullptr;
	}

	bool isTriggered() {
		targetState = targetState.makeDecision(decisionTreeRoot);
		return targetState != nullptr;
	}
};

template <typename T>
struct StateMachine {
	State initialState;
	State currentState;
	TargetState targetState;
	std::vector<Action> actions;

	StateMachine(States startingState) : initialState(startingState), currentState(startingState) {}

	// Should be able to accept any type (Why we use templates)
	// of agent that implements the interface
	std::vector<Action> update(T& agent) {
		DecisionTreeTransition* triggered = nullptr;

		for (auto transition : currentState.getTransitions()) {
			if (transition.isTriggered()) {
				triggered = transition;
				break;
			}
		}
		if (triggered != nullptr) {
			targetState = triggered.getTargetState();
			actions.clear();

			actions[0] = currentState.getActions();
			actions.push_back(triggered.getActions());
			actions.push_back(targetState.getActions());

			currentState = targetState;
			return actions;
		}
		else {
			return currentState.getActions();
		}
	}
};


