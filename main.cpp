#include "Game.h"
#include <iostream>

int main(int argc, char** argv)
{
	// Launches the Ogre Context and starts rendering
	try
	{
		Game game;
		game.initApp();
		game.getRoot()->startRendering();
		game.closeApp();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error occurred during execution: " << e.what() << '\n';
		return 1;
	}
	return 0;
}