#ifndef _TILE_H
#define _TILE_H

#include <iostream>
#include <string>
#include <Ogre.h>
#include "SystemDefines.h"

class Tile
{
public:
	Tile();
	Tile(Ogre::Vector3 position, Ogre::SceneManager* manager, int type, double scale);
	~Tile();
	Ogre::Vector3 getPosition();
	void setPosition(Ogre::Vector3);
	bool isOccupied();
	void setOccupied(bool a);
	double getScale();

	// Used for pathfinding in unit class
	void setG(int g);
	void setH(int h);
	void setF(int f);

	int getG();
	int getH();
	int getF();

	void setParentTile(Tile* parentTile);
	Tile* getParentTile();
	Ogre::Vector2 getGridPosition();
	void setGridPosition(Ogre::Vector2 pos);
	int getType();
	Ogre::Entity* getEntity();

private:
	Ogre::Vector3 position;
	Ogre::SceneManager* sceneManager;
	int type;

	Ogre::Entity* entity;
	std::string name;
	double scale;
	bool occupied;

	// Used for pathfinding in unit class
	int G;
	int H;
	int F;
	Tile* parentTile;
	Ogre::Vector2 gridPosition;
};

#endif