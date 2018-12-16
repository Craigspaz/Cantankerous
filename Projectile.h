#ifndef _PROJECTILE_H
#define _PROJECTILE_H

#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreInput.h>
#include <OgreRTShaderSystem.h>
#include <OgreCameraMan.h>
#include <OgreTrays.h>
#include <OgreAdvancedRenderControls.h>
#include <OgreConfigDialog.h>

class Unit;
class Building;

class Projectile
{
public:
	Projectile(Ogre::Vector3 startingPosition, double damage, double missRate, Ogre::SceneManager* manager, double movementSpeed, int controllingPlayer, bool lockedOn=false, int id=-1);
	~Projectile();
	void update();
	void setTarget(Unit* unit);
	void setTarget(Building* building);

	void setPosition(Ogre::Vector3 position);
	Ogre::Vector3 getPosition();
	bool isDestroyed();
	void setDestroyed(bool a);
	int getID();
	int getControllingPlayer();
	void setVisible(bool a);

private:
	Ogre::SceneNode* node;
	Ogre::Entity* entity;
	Ogre::SceneManager* sceneManager;
	double damage;
	double missRate;
	double movementSpeed;
	int id;
	int controllingPlayer;

	Unit* unitTarget;
	Building* buildingTarget;
	bool lockedOn;
	bool destroyed;
};

#endif