#ifndef _LEVEL_H
#define _LEVEL_H

#include <iostream>
#include <string>
#include <Ogre.h>

class Level
{
public:
	Level(std::string filename, Ogre::SceneManager* manager);
	~Level();


private:

	void loadLevel();


	std::string filename;
	Ogre::SceneManager* sceneManager;
};

#endif