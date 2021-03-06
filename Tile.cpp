#include "Tile.h"


std::string getNewName() // return a unique name
{
	static int count = 0;	// keep counting the number of objects
	std::stringstream out;	// a stream for outputing to a string
	out << count++;			// make the current count into a string
	return "tile_" + out.str();	// append the current count onto the string
}

Tile::Tile()
{
	// Default constructor
}

Tile::Tile(Ogre::Vector3 position, Ogre::SceneManager* manager, int type, double scale)
{
	occupied = false;
	position.x *= scale;
	position.z *= scale;
	this->position = position;
	this->sceneManager = manager;
	this->type = type;
	this->scale = scale;
	//floorPlane = new Ogre::Plane(Ogre::Vector3::UNIT_Y, 0);
	this->node = manager->getRootSceneNode()->createChildSceneNode();
	this->name = getNewName();
	//Ogre::MeshManager::getSingleton().createPlane(this->name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, *floorPlane, scale, scale, 1, 1, true, 1, 1.0, 1.0, Ogre::Vector3::UNIT_Z);
	this->entity = manager->createEntity(this->name, Ogre::SceneManager::PT_PLANE);
	this->node->attachObject(entity);
	this->node->setScale(this->node->getScale() / 200.0);
	this->node->scale(Ogre::Vector3(scale, scale, scale));
	this->node->setOrientation(Ogre::Vector3::UNIT_Z.getRotationTo(Ogre::Vector3::UNIT_Y));
	this->node->setPosition(position);

	if (type == GRASS_TILE)
	{
		this->entity->setMaterialName("Examples/GrassFloor");
	}
	else if (type == DIRT_TILE)
	{
		this->entity->setMaterialName("CraigDirt/DirtFloor");
	}
	else
	{
		this->entity->setMaterialName("Examples/Rockwall");
	}
	G = 0;
	F = 0;
	H = 0;
}

Tile::~Tile()
{
	sceneManager->destroyEntity(this->entity);
	sceneManager->getRootSceneNode()->removeAndDestroyChild(this->node);
}

Ogre::Vector3 Tile::getPosition()
{
	return this->position;
}

void Tile::setPosition(Ogre::Vector3 pos)
{
	this->position = pos;
}


bool Tile::isOccupied()
{
	return occupied;
}


void Tile::setOccupied(bool a)
{
	occupied = a;
}

double Tile::getScale()
{
	return scale;
}

void Tile::setG(int g)
{
	this->G = g;
}
void Tile::setH(int h)
{
	this->H = h;
}
void Tile::setF(int f)
{
	this->F = f;
}

int Tile::getG()
{
	return G;
}

int Tile::getH()
{
	return H;
}

int Tile::getF()
{
	return F;
}

void Tile::setParentTile(Tile* parentTile)
{
	this->parentTile = parentTile;
}
Tile* Tile::getParentTile()
{
	return this->parentTile;
}


Ogre::Vector2 Tile::getGridPosition()
{
	return this->gridPosition;
}
void Tile::setGridPosition(Ogre::Vector2 pos)
{
	this->gridPosition = pos;
}

int Tile::getType()
{
	return type;
}


Ogre::Entity* Tile::getEntity()
{
	return this->entity;
}