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

	// TODO: Pathfinding
}

bool Unit::findPath(Tile*** tiles, Tile* endTile, int width, int height)
{
	if (!path.empty())
	{
		std::list<Tile*> openList;
		std::list<Tile*> closedList;
		Tile* startTile = tiles[(int)currentTile->getGridPosition().x][(int)currentTile->getGridPosition().y];
		
		while (!openList.empty())
		{
			Tile* currentTile = openList.front();
			openList.pop_front();
			closedList.push_back(currentTile);
			// Checks if the tile with the lowest F cost is the destination
			if (currentTile->getGridPosition().x - endTile->getGridPosition().x < 0.001 && currentTile->getGridPosition().y - endTile->getGridPosition().y < 0.001)
			{
				endTile->setParentTile(currentTile);
			}

			Tile* neighbors[8];

			// If there is an adjacent upper left corner tile
			if (currentTile->getGridPosition().x > 0 && currentTile->getGridPosition().y > 0)
			{
				neighbors[0] = tiles[(int)currentTile->getGridPosition().x - 1][(int)currentTile->getGridPosition().y - 1];
			}
			else
			{
				neighbors[0] = NULL;
			}
			// If there is an adjacent upper tile
			if (currentTile->getGridPosition().y > 0)
			{
				neighbors[1] = tiles[(int)currentTile->getGridPosition().x][(int)currentTile->getGridPosition().y - 1];
			}
			else
			{
				neighbors[1] = NULL;
			}

			// If there is an adjacent upper right corner tile
			if (currentTile->getGridPosition().x + 1 < width && currentTile->getGridPosition().y > 0)
			{
				neighbors[2] = tiles[(int)currentTile->getGridPosition().x + 1][(int)currentTile->getGridPosition().y - 1];
			}
			else
			{
				neighbors[2] = NULL;
			}

			// if there is an adjacent left tile
			if (currentTile->getGridPosition().x > 0)
			{
				neighbors[3] = tiles[(int)currentTile->getGridPosition().x - 1][(int)currentTile->getGridPosition().y];
			}
			else
			{
				neighbors[3] = NULL;
			}
			
			// if there is an adjacent right tile
			if (currentTile->getGridPosition().x + 1< width)
			{
				neighbors[4] = tiles[(int)currentTile->getGridPosition().x + 1][(int)currentTile->getGridPosition().y];
			}
			else
			{
				neighbors[4] = NULL;
			}

			//if there is an adjacent lower left tile
			if (currentTile->getGridPosition().x > 0 && currentTile->getGridPosition().y + 1< height)
			{
				neighbors[5] = tiles[(int)currentTile->getGridPosition().x - 1][(int)currentTile->getGridPosition().y + 1];
			}
			else
			{
				neighbors[5] = NULL;
			}

			// if there is an adjacent lower tile
			if (currentTile->getGridPosition().y + 1 < height)
			{
				neighbors[6] = tiles[(int)currentTile->getGridPosition().x][(int)currentTile->getGridPosition().y + 1];
			}
			else
			{
				neighbors[6] = NULL;
			}

			// if there is an adjacent lower right corner
			if (currentTile->getGridPosition().x + 1 < width && currentTile->getGridPosition().y + 1 < height)
			{
				neighbors[7] = tiles[(int)currentTile->getGridPosition().x + 1][(int)currentTile->getGridPosition().y + 1];
			}
			else
			{
				neighbors[7] = NULL;
			}

			for (int i = 0; i < 8; i++)
			{
				if (neighbors[i] != NULL)
				{
					if (!(std::find(openList.begin(), openList.end(), neighbors[i]) != openList.end() || currentTile->getG() + 10))
					{
						neighbors[i]->setParentTile(currentTile);
						neighbors[i]->setG(currentTile->getG() + 10);
						neighbors[i]->setH(abs(neighbors[i]->getGridPosition().x - endTile->getGridPosition().x) + abs(neighbors[i]->getGridPosition().y - endTile->getGridPosition().y));
						neighbors[i]->setF(neighbors[i]->getG() + neighbors[i]->getH());
						for (std::list<Tile*>::iterator it = openList.begin(); it != openList.end(); it++)
						{
							if (neighbors[i]->getF() < (*it)->getF())
							{
								openList.insert(it, neighbors[i]);
								break;
							}
						}
					}
				}
			}
		}
	}
	return false;
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
