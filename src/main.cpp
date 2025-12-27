
#include "raylib.h"

#include "resource_dir.h"	
#include "AIHeader.h"

// Worker struct 
int main ()
{
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
