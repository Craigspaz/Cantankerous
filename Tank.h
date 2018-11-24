#ifndef _TANK_H
#define _TANK_H
#include "Unit.h"
#include "Building.h"

class Tank : public Unit
{
public:
	Tank(Ogre::Vector3 position, Ogre::SceneManager* sceneManager, int controlledByPlayerNumber, int id=-1);
	~Tank();

	virtual void update(Level* level);
	void attack(Unit* target);
	//void attack(Building* building); // may need to change

	Ogre::Entity* getTurretEntity();
	Ogre::Entity* getBaseEntity();

private:
	Ogre::SceneNode* turretNode;
	Ogre::SceneNode* baseNode;
	Ogre::Entity* turret;
	Ogre::Entity* base;
};

#endif