#ifndef _UNIT_H
#define _UNIT_H

#include <Ogre.h>
class Unit
{
public:
	Unit(Ogre::Vector3 position, Ogre::Entity* entity, Ogre::SceneManager* sceneManager, int controlledByPlayerNumber);
	~Unit();

	void setPosition(Ogre::Vector3 position);
	Ogre::Vector3 getPosition();
	void update();

	int getPlayerControlledBy();
	void setPlayerControlledBy(int i);

private:
	Ogre::SceneNode* node;
	Ogre::SceneManager* manager;
	int controlledByPlayerNumber;
};

#endif