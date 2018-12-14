#ifndef _BUILDING_H
#define _BUILDING_H

#include <iostream>
#include <string>
#include "SystemDefines.h"
#include <Ogre.h>
#include <OgreTrays.h>
#include <OgreAdvancedRenderControls.h>

class Building
{
public:
	Building(Ogre::Vector3 position, Ogre::SceneManager* sceneManager, int controlledByPlayerNumber, int type, int id=-1);
	~Building();

	Ogre::Vector3 getPosition();
	Ogre::Entity* getEntity();
	int getControllingPlayerID();
	int getType();
	int getID();
	void setVisible(bool value);
	void setPosition(Ogre::Vector3 pos);
	void setSelected(bool value, OgreBites::TrayManager* trayManager);

private:
	Ogre::Entity* entity;
	Ogre::SceneManager* sceneManager;
	int controlledByPlayerNumber;
	int type;
	Ogre::SceneNode* node;
	int id;
	bool alreadySelected;
};

#endif