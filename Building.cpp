#include "Building.h"

int generateID() // return a unique name
{
	static int count = 0;	// keep counting the number of objects
	count++;
	return count;	// append the current count onto the string
}


Building::Building(Ogre::Vector3 position, Ogre::SceneManager* sceneManager, int controlledByPlayerNumber, int type, int id)
{
	this->sceneManager = sceneManager;
	this->controlledByPlayerNumber = controlledByPlayerNumber;
	this->type = type;
	this->node = sceneManager->getRootSceneNode()->createChildSceneNode();
	this->node->setPosition(position);
	this->entity = sceneManager->createEntity("tudorhouse.mesh");
	node->attachObject(entity);
	node->setScale(node->getScale() / 40.0);
	if (id == -1)
	{
		this->id = generateID();
	}
	else
	{
		this->id = id;
	}
}


Building::~Building()
{
}

Ogre::Vector3 Building::getPosition()
{
	return this->node->getPosition();
}

Ogre::Entity* Building::getEntity()
{
	return this->entity;
}

int Building::getControllingPlayerID()
{
	return this->controlledByPlayerNumber;
}

int Building::getType()
{
	return this->type;
}

int Building::getID()
{
	return this->id;
}