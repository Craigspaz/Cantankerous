#ifndef _GAME_H
#define _GAME_H

#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreInput.h>
#include <OgreRTShaderSystem.h>
#include <OgreCameraMan.h>
#include <OgreTrays.h>
#include <OgreAdvancedRenderControls.h>
#include <OgreConfigDialog.h>
#include <iostream>
#include "Client.h"
#include "Server.h"

using namespace Ogre;
using namespace OgreBites;

class Game : public ApplicationContext, public InputListener, public TrayListener
{
public:
	Game();
	~Game();
	
	void setup();
	
	bool keyPressed(const KeyboardEvent& event);
	bool keyReleased(const KeyboardEvent& event);
	
	bool frameRenderingQueued(const Ogre::FrameEvent& event);
	bool mouseMoved(const MouseMotionEvent& event);
	bool mousePressed(const MouseButtonEvent& event);
	bool mouseReleased(const MouseButtonEvent& event);
	
	bool touchReleased(const TouchFingerEvent& event);
	bool mouseWheelRolled(const MouseWheelEvent& event);

	virtual void buttonHit(Button* button) override;

	void update();
	
private:

	SceneManager* sceneManager;
	Ogre::Camera* camera;
	Ogre::SceneNode* cameraNode;
	OgreBites::TrayManager* trayManager;
	OgreBites::AdvancedRenderControls* controls;
	OgreBites::CameraMan* cameraMan;
	
	bool keys[255];
	bool mouseButtons[10];
	char gameMode;

	Server* server;
	Client* client;
};	

#endif