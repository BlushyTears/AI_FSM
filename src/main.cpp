
#include "raylib.h"
#include "FSMLibrary.h"
#include <vector>

#include "resource_dir.h"	
#include "AgentNew.h"
#include "Clock.h"

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

const int MAXIMUM_AGENTS = 4;
// Worker struct 
int main ()
{
	std::vector<Agent<int>> agentList;
	for (int i = 0; i < MAXIMUM_AGENTS; i++) {
		Agent<int> agent;
		agentList.push_back(agent);
	}

	for (auto& agent : agentList) {
		agent.sm.update(agent);
	}

	InitWindow(1280, 800, "Hello Raylib");

	Clock c;
	
	while (!WindowShouldClose())		
	{
		BeginDrawing();
		ClearBackground(BLACK);
		c.update();
		c.display();
		EndDrawing();
	}

	CloseWindow();
	return 0;
}

