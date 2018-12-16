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

class Projectile
{
public:
	Projectile(Ogre::Vector3 startingPosition, double damage, double missRate);
	~Projectile();


private:
	Ogre::SceneNode* node;
	Ogre::Entity* entity;
	double damage;
	double missRate;
};

#endif