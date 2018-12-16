#ifndef _TANK_H
#define _TANK_H
#include "Unit.h"
#include "Building.h"

class Tank : public Unit
{
public:
	Tank(Ogre::Vector3 position, Ogre::SceneManager* sceneManager, int controlledByPlayerNumber, int id=-1);
	~Tank();

	virtual void update(Level* level, std::vector<Projectile*>* projectiles);
	virtual void attack(std::vector<Projectile*>* projectiles);

	Ogre::Entity* getTurretEntity();
	Ogre::Entity* getBaseEntity();
	void setVisible(bool value);

private:
	Ogre::SceneNode* turretNode;
	Ogre::SceneNode* baseNode;
	Ogre::Entity* turret;
	Ogre::Entity* base;
	int firingCoolOffAmount;
	int currentTicks;
};

#endif