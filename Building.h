#ifndef _BUILDING_H
#define _BUILDING_H

#include <iostream>
#include <string>
#include "SystemDefines.h"
#include <Ogre.h>

class Building
{
public:
	Building(Ogre::Vector3 position, Ogre::Entity* entity, Ogre::SceneManager* sceneManager, int controlledByPlayerNumber, int type);
	~Building();
};

#endif