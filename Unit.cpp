#include "Unit.h"



Unit::Unit(Ogre::Vector3 position, Ogre::Entity* entity, Ogre::SceneManager* sceneManager, int controlledByPlayerNumber)
{
	Ogre::SceneNode* sceneNode = sceneManager->getRootSceneNode()->createChildSceneNode();
	this->node = sceneNode;
	this->node->attachObject(entity);
	this->manager = sceneManager;
	this->controlledByPlayerNumber = controlledByPlayerNumber;
}


Unit::~Unit()
{
	this->manager->getRootSceneNode()->removeAndDestroyChild(node);
}


void Unit::setPosition(Ogre::Vector3 position)
{
	node->setPosition(position);
}

Ogre::Vector3 Unit::getPosition()
{
	return node->getPosition();
}

void Unit::update()
{

}


int Unit::getPlayerControlledBy()
{
	return this->controlledByPlayerNumber;
}
void Unit::setPlayerControlledBy(int i)
{
	this->controlledByPlayerNumber = i;
}
