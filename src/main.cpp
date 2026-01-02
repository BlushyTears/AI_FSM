
#include "raylib.h"
#include "FSMLibrary.h"
#include <vector>

#include "resource_dir.h"	
#include "AgentNew.h"
#include "Clock.h"


template <typename T>
struct IdleAction : Action<T> {
	void execute(T& agent) override {
		std::cout << "Agent" << agent.id << " Is idleing.. " << std::endl;
	}
};

template <typename T>
struct IdleState : State<T> {
	IdleAction<T> idle;
	
	std::vector<Action<T>*> getActions() override { return { &idle }; };
	std::vector<Action<T>*> getEntryActions() override { return {}; };
	std::vector<Action<T>*> getExitActions() override { return {}; };
	std::vector<Transition<T>*> getTransitions() override { return {}; };
};

const int MAXIMUM_AGENTS = 4;
// Worker struct 
int main ()
{
	IdleState<Agent<int>>* startPoint = new IdleState<Agent<int>>();

	Agent<int> bob;
	bob.id = 1;
	bob.sm = StateMachine<Agent<int>>(startPoint);

	std::cout << "Initating..." << std::endl;

	for (int i = 0; i < MAXIMUM_AGENTS; i++) {
		std::vector<Action<Agent<int>>*> plan = bob.sm.update(bob);

		for (auto* action : plan) {
			action->execute(bob);
		}
	}

	delete startPoint;
	return 0;

	//InitWindow(1280, 800, "Hello Raylib");

	//Clock c;
	//
	//while (!WindowShouldClose())		
	//{
	//	BeginDrawing();
	//	ClearBackground(BLACK);
	//	c.update();
	//	c.display();
	//	EndDrawing();
	//}

	//CloseWindow();
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