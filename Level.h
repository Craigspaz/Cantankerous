#ifndef _LEVEL_H
#define _LEVEL_H

#include <iostream>
#include <string>
#include <Ogre.h>
#include "Tile.h"

class Level
{
public:
	Level(std::string filename, Ogre::SceneManager* manager);
	~Level();

	Ogre::Vector3 getMinBoundary();
	Ogre::Vector3 getMaxBoundary();

private:

	void loadLevel();


	std::string filename;
	Ogre::SceneManager* sceneManager;
	std::vector<Tile*>* tiles;
	
	Ogre::Vector3 minCorner;
	Ogre::Vector3 maxCorner;
};

#endif