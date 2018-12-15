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
	alreadySelected = false;
	buildQueue = new std::vector<int>();
	sizeOfQueue = 0;
	maxSizeOfBuildQueue = MAX_SIZE_OF_BUILD_QUEUE;
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

void Building::setVisible(bool value)
{
	this->node->setVisible(value);
}


void Building::setPosition(Ogre::Vector3 pos)
{
	this->node->setPosition(pos);
}

void Building::setSelected(bool value, OgreBites::TrayManager* trayManager)
{
	if (value == true)
	{
		if (!alreadySelected)
		{
			trayManager->createButton(OgreBites::TL_BOTTOM, "SpawnTank", "Spawn Tank");
		}
		alreadySelected = true;
	}
	else
	{
		if (alreadySelected)
		{
			alreadySelected = false;
			trayManager->destroyAllWidgets();
		}
	}
}


void Building::addUnitToQueue(int type)
{
	buildQueueLock.lock();
	if (sizeOfQueue + 1 > maxSizeOfBuildQueue)
	{
		buildQueue->push_back(type);
		sizeOfQueue++;
	}
	buildQueueLock.unlock();
}


void Building::removeFirstItemFromQueue()
{
	buildQueueLock.lock();
	if (sizeOfQueue > 0)
	{
		for (int i = 0; i < sizeOfQueue && i + 1 < sizeOfQueue; i++)
		{
			buildQueue[i] = buildQueue[i + 1];
		}
		sizeOfQueue--;
	}
	buildQueueLock.unlock();
}

std::vector<int>* Building::getQueue()
{
	return buildQueue;
}


int Building::getCurrentSizeOfQueue()
{
	buildQueueLock.lock();
	int size = sizeOfQueue;
	buildQueueLock.unlock();
	return size;
}


void Building::setQueue(std::vector<int> queue)
{
	buildQueueLock.lock();
	buildQueue->clear();
	sizeOfQueue = 0;
	for (auto item : queue)
	{
		buildQueue->push_back(item);
		sizeOfQueue++;
	}
	buildQueueLock.unlock();
}