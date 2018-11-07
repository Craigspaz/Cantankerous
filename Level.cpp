#include "Level.h"

Level::Level(std::string filename, Ogre::SceneManager* manager)
{
	this->filename = filename;
	this->sceneManager = manager;

	loadLevel();
}

Level::~Level()
{
	
}

void Level::loadLevel()
{

}