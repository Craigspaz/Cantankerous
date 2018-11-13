#ifndef _TANK_H
#define _TANK_H
#include "Unit.h"

class Tank : public Unit
{
public:
	Tank(Ogre::Vector3 position, Ogre::SceneManager* sceneManager, int controlledByPlayerNumber);
	~Tank();

	virtual void update();

private:
	Ogre::SceneNode* turretNode;
	Ogre::SceneNode* baseNode;
	Ogre::Entity* turret;
	Ogre::Entity* base;
};

#endif