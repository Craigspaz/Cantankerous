#include "Tile.h"


std::string getNewName() // return a unique name
{
	static int count = 0;	// keep counting the number of objects
	std::stringstream out;	// a stream for outputing to a string
	out << count++;			// make the current count into a string
	return "tile_" + out.str();	// append the current count onto the string
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
	Ogre::Plane floorPlane(Ogre::Vector3::UNIT_Y, 0);
	this->name = getNewName();
	Ogre::MeshManager::getSingleton().createPlane(this->name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, floorPlane, scale, scale, 1, 1, true, 1, 1.0, 1.0, Ogre::Vector3::UNIT_Z);
	this->entity = manager->createEntity(this->name);

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
	Ogre::SceneNode* node = manager->getRootSceneNode()->createChildSceneNode();
	node->attachObject(this->entity);
	node->setPosition(position);
}

Tile::~Tile()
{
	sceneManager->destroyEntity(this->entity);
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