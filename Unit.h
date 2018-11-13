#ifndef _UNIT_H
#define _UNIT_H

#include <Ogre.h>
class Unit
{
public:
	Unit(Ogre::Vector3 position, Ogre::SceneManager* sceneManager, int controlledByPlayerNumber, int type);
	~Unit();

	void setPosition(Ogre::Vector3 position);
	Ogre::Vector3 getPosition();
	virtual void update();
	int getType();

	int getPlayerControlledBy();
	void setPlayerControlledBy(int i);

protected:
	Ogre::SceneNode* node;
	Ogre::SceneManager* manager;
	int controlledByPlayerNumber;
};

#endif