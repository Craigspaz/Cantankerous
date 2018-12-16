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
	buildQueue = new int[MAX_SIZE_OF_BUILD_QUEUE];
	sizeOfQueue = 0;
	maxSizeOfBuildQueue = MAX_SIZE_OF_BUILD_QUEUE;
	ticksPassed = 0;
	health = 200;
	destroyed = false;
}


Building::~Building()
{
	delete buildQueue;
}

int Building::update()
{
	lock();
	buildQueueLock.lock();
	if (sizeOfQueue == 0)
	{
		ticksPassed = 0;
		buildQueueLock.unlock();
		unlock();
		return -1;
	}
	buildQueueLock.unlock();

	if (ticksPassed > TICKS_TILL_UNIT_CREATION)
	{
		int unitToCreate = -1;
		if (sizeOfQueue > 0)
		{
			unitToCreate = this->buildQueue[0];
			this->removeFirstItemFromQueue();
		}
		ticksPassed = 0;
		unlock();
		return unitToCreate;
	}
	else
	{
		std::cout << "Ticking..." << std::endl;
		ticksPassed++;
		unlock();
		return -1;
	}
	unlock();
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
	lock();
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
	unlock();
}


void Building::addUnitToQueue(int type)
{
	lock();
	buildQueueLock.lock();
	if (sizeOfQueue + 1 < maxSizeOfBuildQueue)
	{
		buildQueue[sizeOfQueue] = type;
		sizeOfQueue++;
	}
	buildQueueLock.unlock();
	unlock();
}


void Building::removeFirstItemFromQueue()
{
	if (sizeOfQueue > 0)
	{
		for (int i = 0; (i + 1) < sizeOfQueue; i++)
		{
			buildQueue[i] = buildQueue[i + 1];
		}
		sizeOfQueue--;
	}
}

int* Building::getQueue()
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
	lock();
	buildQueueLock.lock();
	sizeOfQueue = 0;
	for (auto item : queue)
	{
		buildQueue[sizeOfQueue] = item;
		sizeOfQueue++;
	}
	buildQueueLock.unlock();
	unlock();
}


void Building::lock()
{
	mutex.lock();
}

void Building::unlock()
{
	mutex.unlock();
}


void Building::takeDamage(int damage)
{
	health -= damage;
}


bool Building::isDestroyed()
{
	return destroyed;
}