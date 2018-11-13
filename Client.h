#ifndef _CLIENT_H
#define _CLIENT_H

#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreInput.h>
#include <OgreRTShaderSystem.h>
#include <OgreCameraMan.h>
#include <OgreTrays.h>
#include <OgreAdvancedRenderControls.h>
#include <OgreConfigDialog.h>
#include <iostream>
#include <string>
#include "Messages.h"
#include "Tile.h"
#include "Tank.h"

class Game;

class Client
{
public:
	Client(std::string ip, int port, Game* game, Ogre::SceneManager* sceneManager);
	~Client();
	void handleClick(const OgreBites::MouseButtonEvent& event);
	void update(Ogre::SceneNode* cameraNode);

private:

	void processInitialMessage(char* message);
	void getInitialInfo();
	void receiveMessages();

	SOCKET sock;
	struct sockaddr_in connection;
	Game* game;
	Ogre::SceneManager* sceneManager;
	std::thread* messageRecievingThread;

	std::vector<Unit*>* localCopyOfUnits;
	std::mutex unitsLock;

};

#endif