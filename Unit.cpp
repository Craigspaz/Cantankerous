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
	directionMoving = Ogre::Vector3(0, 0, 1);
	this->currentTile = NULL;
	if (id == -1)
	{
		this->id = getID();
	}
	else
	{
		this->id = id;
	}
	path = new std::list<Tile*>();
	isMovingAlongPath = false;
	this->health = 100;
	this->damage = 10;
	selectionNode = NULL;
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
	if (!path->empty() && this->isMovingAlongPath)
	{
		Tile* nextTile = path->front();
		if (abs(this->getPosition().x - nextTile->getPosition().x) <= movementSpeed && abs(this->getPosition().z - nextTile->getPosition().z) <= movementSpeed)
		{
			currentTile = nextTile;
			path->pop_front();
			if (!path->empty())
			{
				nextTile = path->front();
			}
			else
			{
				this->isMovingAlongPath = false;
				return;
			}
		}

		if (this->getPosition().x - nextTile->getPosition().x > 0)
		{
			// move left
			this->setPosition(this->getPosition() + Ogre::Vector3(-movementSpeed, 0, 0));
		}
		else if (this->getPosition().x - nextTile->getPosition().x < 0)
		{
			// move right
			this->setPosition(this->getPosition() + Ogre::Vector3(movementSpeed, 0, 0));
		}

		if (this->getPosition().z - nextTile->getPosition().z > 0)
		{
			// move up
			this->setPosition(this->getPosition() + Ogre::Vector3(0, 0, -movementSpeed));
		}
		else if (this->getPosition().z - nextTile->getPosition().z < 0)
		{
			// move down
			this->setPosition(this->getPosition() + Ogre::Vector3(0, 0, movementSpeed));
		}
		directionMoving = nextTile->getPosition() - this->currentTile->getPosition();
		this->node->setOrientation(Ogre::Vector3::UNIT_Z.getRotationTo(this->directionMoving));
	}
}

std::list<Tile*>* Unit::findPath(Tile*** tiles, Tile* endTile, int width, int height)
{
	std::list<Tile*>* newPath = new std::list<Tile*>();
	
	std::list<Tile*> openList;
	std::list<Tile*> closedList;
	Tile* startTile = tiles[(int)this->currentTile->getGridPosition().x][(int)this->currentTile->getGridPosition().y];
	startTile->setParentTile(NULL);
	openList.push_back(startTile);
	while (!openList.empty())
	{
		Tile* currentTile = openList.front();
		openList.pop_front();
		closedList.push_back(currentTile);
		// Checks if the tile with the lowest F cost is the destination
		if (abs(currentTile->getGridPosition().x - endTile->getGridPosition().x) < 0.001 && abs(currentTile->getGridPosition().y - endTile->getGridPosition().y) < 0.001)
		{
			//endTile->setParentTile(currentTile);
			Tile* tmpTile = endTile;
			while (tmpTile != NULL && tmpTile != startTile)
			{
				newPath->push_front(tmpTile);
				tmpTile = tmpTile->getParentTile();
			}
			return newPath;
		}
		else
		{
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
			if (currentTile->getGridPosition().x + 1 < width)
			{
				neighbors[4] = tiles[(int)currentTile->getGridPosition().x + 1][(int)currentTile->getGridPosition().y];
			}
			else
			{
				neighbors[4] = NULL;
			}

			//if there is an adjacent lower left tile
			if (currentTile->getGridPosition().x > 0 && currentTile->getGridPosition().y + 1 < height)
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

			int cost = 10;
			for (int i = 0; i < 8; i++)
			{
				if (neighbors[i] != NULL)
				{
					if (i == 0 || i == 2 || i == 5 || i == 7)
					{
						cost = 14;
					}
					else
					{
						cost = 10;
					}
					if (!neighbors[i]->isOccupied() && !(std::find(closedList.begin(), closedList.end(), neighbors[i]) != closedList.end()))
					{
						if (!(std::find(openList.begin(), openList.end(), neighbors[i]) != openList.end()) || currentTile->getG() + cost < neighbors[i]->getG())
						{
							neighbors[i]->setParentTile(currentTile);
							neighbors[i]->setG(currentTile->getG() + cost);
							neighbors[i]->setH(abs(neighbors[i]->getGridPosition().x - endTile->getGridPosition().x) + abs(neighbors[i]->getGridPosition().y - endTile->getGridPosition().y));
							neighbors[i]->setF(neighbors[i]->getG() + neighbors[i]->getH());
							if (openList.empty())
							{
								openList.push_front(neighbors[i]);
							}
							else
							{
								bool inserted = false;
								for (std::list<Tile*>::iterator it = openList.begin(); it != openList.end(); it++)
								{
									if (neighbors[i]->getF() < (*it)->getF())
									{
										openList.insert(it, neighbors[i]);
										inserted = true;
										break;
									}
								}
								if (!inserted)
								{
									openList.push_back(neighbors[i]);
								}
							}
						}
					}
				}
			}
		}
	}
	return newPath;
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



int Unit::getUnitID()
{
	return id;
}


Ogre::Quaternion Unit::getOrientation()
{
	return this->node->getOrientation();
}


void Unit::setOrientation(Ogre::Quaternion a)
{
	this->node->setOrientation(a);
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

bool Unit::isMoving()
{
	return this->isMovingAlongPath;
}


void Unit::setDestination(Tile* tile, Level* level)
{
	isMovingAlongPath = true;
	Tile*** copyOfTiles = new Tile**[level->getWidth()];
	for (int x = 0; x < level->getWidth(); x++)
	{
		copyOfTiles[x] = new Tile*[level->getHeight()];
	}
	Tile* copyOfEndTile = NULL;
	int x = 0;
	int y = 0;
	for (std::vector<Tile*>::iterator i = level->getTiles()->begin(); i != level->getTiles()->end(); i++)
	{
		copyOfTiles[x][y] = new Tile((*i)->getPosition(), this->manager, (*i)->getType(), (*i)->getScale());
		copyOfTiles[x][y]->setGridPosition((*i)->getGridPosition());
		if (tile->getGridPosition().x == copyOfTiles[x][y]->getGridPosition().x && tile->getGridPosition().y == copyOfTiles[x][y]->getGridPosition().y)
		{
			copyOfEndTile = copyOfTiles[x][y];
		}
		x++;
		if (x >= level->getWidth())
		{
			x = 0;
			y++;
		}
	}
	std::list<Tile*>* nPath = findPath(copyOfTiles, copyOfEndTile, level->getWidth(), level->getHeight());
	path->clear();
	while (!nPath->empty())
	{
		Tile* front = nPath->front();
		for (std::vector<Tile*>::iterator i = level->getTiles()->begin(); i != level->getTiles()->end(); i++)
		{
			if (front->getGridPosition().x == (*i)->getGridPosition().x && front->getGridPosition().y == (*i)->getGridPosition().y)
			{
				path->push_back((*i));
				break;
			}
		}
		nPath->pop_front();
	}
	delete nPath;
	for (int x = 0; x < level->getWidth(); x++)
	{
		for (int y = 0; y < level->getHeight(); y++)
		{
			delete copyOfTiles[x][y];
		}
		delete copyOfTiles[x];
	}
	delete copyOfTiles;
}

Ogre::Vector3 Unit::getDirectionMoving()
{
	return this->directionMoving;
}


void Unit::setSelected(bool value)
{
	if (value == true && selectionNode == NULL)
	{
		Ogre::SceneNode* selectNode = this->node->createChildSceneNode();
		selectNode->setPosition(Ogre::Vector3(selectNode->getPosition().x, 20, selectNode->getPosition().z));
		Ogre::Entity* entity = this->manager->createEntity(Ogre::SceneManager::PT_CUBE);
		selectNode->attachObject(entity);
		selectNode->setScale(selectNode->getScale() / 50.0);
	}
	else
	{
		if (selectionNode == NULL)
		{
			return;
		}
		selectionNode->removeAndDestroyAllChildren();
		selectionNode = NULL;
	}
}