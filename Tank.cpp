#include "Tank.h"
#include "SystemDefines.h"
#include <Ogre.h>



Tank::Tank(Ogre::Vector3 position, Ogre::SceneManager* sceneManager, int controlledByPlayerNumber, int id) : Unit(position,sceneManager,controlledByPlayerNumber, UNIT_TANK, id)
{
	Ogre::Entity* tmpEntity = sceneManager->createEntity("treds_Cube.mesh");
	this->baseNode = this->node->createChildSceneNode();
	this->baseNode->scale(Ogre::Vector3(3, 3, 3));
	this->baseNode->attachObject(tmpEntity);
	this->base = tmpEntity;

	tmpEntity = sceneManager->createEntity("turret_Cube.001.mesh");
	this->turretNode = this->baseNode->createChildSceneNode();
	//this->turretNode->scale(Ogre::Vector3(3, 3, 3));
	this->turretNode->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian(Ogre::Degree(-78)));
	this->turretNode->attachObject(tmpEntity);
	this->turret = tmpEntity;
	this->health = 100 + rand() % 100;
	this->damage = 5 + rand() % 10;

	baseNode->setOrientation((baseNode->getOrientation() * Ogre::Vector3::UNIT_Z).getRotationTo(Ogre::Vector3::UNIT_Z));
	// For debugging
	//this->turretNode->showBoundingBox(true);
	//this->baseNode->showBoundingBox(true);
	currentTicks = 120;
	firingCoolOffAmount = 120;
}


void Tank::update(Level* level, std::vector<Projectile*>* projectiles)
{
	Unit::update(level, projectiles);

	//TODO handle rotating turret
}

Tank::~Tank()
{
	this->node->removeAndDestroyAllChildren();
	this->manager->getRootSceneNode()->removeAndDestroyChild(this->node);
	this->manager->destroyEntity(this->base);
	this->manager->destroyEntity(this->turret);
}


void Tank::attack(std::vector<Projectile*>* projectiles)
{
	if (this->targetUnit != NULL && currentTicks >= firingCoolOffAmount)
	{
		// fire projectile at target
		Projectile* projectile = new Projectile(this->getPosition(), this->damage + rand() % 30, 0.1, this->manager, 2, false, this->controlledByPlayerNumber);
		projectiles->push_back(projectile);
		projectile->setTarget(targetUnit);
		currentTicks = 0;
	}
	else if (this->targetBuilding != NULL && currentTicks >= firingCoolOffAmount)
	{
		// fire projectile at target
		Projectile* projectile = new Projectile(this->getPosition(), this->damage + rand() % 30, 0.1, this->manager, 2, false, this->controlledByPlayerNumber);
		projectiles->push_back(projectile);
		projectile->setTarget(targetBuilding, targetBuildingTile);
		currentTicks = 0;
	}
	else
	{
		currentTicks++;
	}
}


Ogre::Entity* Tank::getTurretEntity()
{
	return turret;
}

Ogre::Entity* Tank::getBaseEntity()
{
	return base;
}


void Tank::setVisible(bool value)
{
	this->base->setVisible(value);
	this->turret->setVisible(value);
}