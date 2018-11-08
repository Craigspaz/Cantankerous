#ifndef _TILE_H
#define _TILE_H

#include <iostream>
#include <string>
#include <Ogre.h>
#include "SystemDefines.h"

class Tile
{
public:
	Tile(Ogre::Vector3 position, Ogre::SceneManager* manager, int type);
	~Tile();
	Ogre::Vector3 getPosition();
	void setPosition(Ogre::Vector3);

private:
	Ogre::Vector3 position;
	Ogre::SceneManager* sceneManager;
	int type;

	Ogre::Entity* entity;
	std::string name;
};

#endif