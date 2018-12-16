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
	void setOrientation(Ogre::Quaternion a);
	Ogre::Vector3 getScale();
	
	virtual void update(Level* level);
	int getType();

	int getPlayerControlledBy();
	void setPlayerControlledBy(int i);
	int getUnitID();

	void setDestination(Tile* tile, Level* level);
	bool isMoving();
	Ogre::Vector3 getDirectionMoving();
	virtual void setVisible(bool value) = 0;
	void setSelected(bool value);
	Tile* getCurrentTile();
	void setTarget(Unit* unit);


	void lock();
	void unlock();

protected:
	std::list<Tile*>* path;
	Tile* currentTile;
	double movementSpeed;
	int id;
	int health;
	int damage;
	Ogre::SceneNode* selectionNode;

	Ogre::SceneNode* node;
	Ogre::SceneManager* manager;
	int controlledByPlayerNumber;
	int type;
	bool isMovingAlongPath;
	Ogre::Vector3 directionMoving;

	std::mutex mutex;

	Unit* target;
	int shootingRange;

private:
	std::list<Tile*>* findPath(Tile*** tiles, Tile* endTile, int width, int height);
};

#endif