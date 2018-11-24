#include "Tank.h"
#include "SystemDefines.h"
#include <Ogre.h>



Tank::Tank(Ogre::Vector3 position, Ogre::SceneManager* sceneManager, int controlledByPlayerNumber, int id) : Unit(position,sceneManager,controlledByPlayerNumber, UNIT_TANK)
{
	Ogre::Entity* tmpEntity = sceneManager->createEntity("treds_Cube.mesh");
	this->baseNode = this->node->createChildSceneNode();
	this->baseNode->scale(Ogre::Vector3(3, 3, 3));
	this->baseNode->attachObject(tmpEntity);
	this->base = tmpEntity;

	tmpEntity = sceneManager->createEntity("turret_Cube.001.mesh");
	this->turretNode = this->node->createChildSceneNode();
	this->turretNode->scale(Ogre::Vector3(3, 3, 3));
	this->turretNode->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian(Ogre::Degree(100)));
	this->turretNode->attachObject(tmpEntity);
	this->turret = tmpEntity;
}


void Tank::update(Level* level)
{
	//this->setPosition(this->getPosition() + Ogre::Vector3(0.5, 0, 0));
}

Tank::~Tank()
{
	this->node->removeAndDestroyAllChildren();
	this->manager->getRootSceneNode()->removeAndDestroyChild(this->node);
	this->manager->destroyEntity(this->base);
	this->manager->destroyEntity(this->turret);
}


void Tank::attack(Unit* target)
{

}


Ogre::Entity* Tank::getTurretEntity()
{
	return turret;
}

Ogre::Entity* Tank::getBaseEntity()
{
	return base;
}
