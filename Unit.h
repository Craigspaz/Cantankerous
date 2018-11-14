#ifndef _UNIT_H
#define _UNIT_H

#include <Ogre.h>
#include "Tile.h"
#include <vector>
#include "Level.h"

class Unit
{
public:
	Unit(Ogre::Vector3 position, Ogre::SceneManager* sceneManager, int controlledByPlayerNumber, int type, int id=-1);
	~Unit();

	void setPosition(Ogre::Vector3 position);
	Ogre::Vector3 getPosition();
	void setRotation(Ogre::Degree rotation);
	void setScale(Ogre::Vector3 scale);
	Ogre::Quaternion getOrientation();
	Ogre::Vector3 getScale();
	
	virtual void update(Level* level);
	int getType();
	void generatePath(Tile* destination, Level* level);

	int getPlayerControlledBy();
	void setPlayerControlledBy(int i);
	int getUnitID();

protected:
	bool checkPath();

	std::vector<Tile*> path;
	Tile* currentTile;
	double movementSpeed;
	int id;

	Ogre::SceneNode* node;
	Ogre::SceneManager* manager;
	int controlledByPlayerNumber;
};

#endif