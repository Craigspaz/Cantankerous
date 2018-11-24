#include "Unit.h"


int getID() // return a unique name
{
	static int count = 0;	// keep counting the number of objects
	count++;
	return count;	// append the current count onto the string
}


Unit::Unit(Ogre::Vector3 position, Ogre::SceneManager* sceneManager, int controlledByPlayerNumber, int type, int id)
{
	Ogre::SceneNode* sceneNode = sceneManager->getRootSceneNode()->createChildSceneNode();
	this->node = sceneNode;
	this->manager = sceneManager;
	this->controlledByPlayerNumber = controlledByPlayerNumber;
	this->type = type;
	movementSpeed = 0.5f;
	if (id == -1)
	{
		this->id = getID();
	}
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

void Unit::update(Level* level)
{
	if (currentTile == NULL)
	{
		for (auto tile : *(level->getTiles()))
		{
			Ogre::Vector3 currentPosition = getPosition();
			Ogre::Vector3 tilesPosition = tile->getPosition();
			if (currentPosition.x > tilesPosition.x - (tile->getScale() / 2) && currentPosition.x < tilesPosition.x + (tile->getScale() / 2)) // if x
			{
				if (currentPosition.z > tilesPosition.z - (tile->getScale() / 2) && currentPosition.z < tilesPosition.z + (tile->getScale() / 2)) // if y
				{
					currentTile = tile;
					break;
				}
			}
		}
	}

	// TODO move unit along path
	if (!path.empty())
	{
		std::vector<Tile*> openList;
		std::vector<Tile*> closedList;
		/*while (!openList.empty())
		{

		}*/
	}
}

int Unit::getType()
{
	return type;
}


int Unit::getPlayerControlledBy()
{
	return this->controlledByPlayerNumber;
}
void Unit::setPlayerControlledBy(int i)
{
	this->controlledByPlayerNumber = i;
}


void Unit::generatePath(Tile* destination, Level* level)
{
	
}


bool Unit::checkPath()
{
	return false;
}


int Unit::getUnitID()
{
	return id;
}


Ogre::Quaternion Unit::getOrientation()
{
	return this->node->getOrientation();
}


Ogre::Vector3 Unit::getScale()
{
	return this->node->getScale();
}


void Unit::setRotation(Ogre::Degree angle)
{
	this->node->rotate(Ogre::Vector3::UNIT_Y, angle);
}


void Unit::setScale(Ogre::Vector3 scale)
{
	this->node->setScale(scale);
}
