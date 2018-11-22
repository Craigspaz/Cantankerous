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
	struct UnitsToCreateData
	{
		int id = -1;
		Ogre::Vector3 position;
		Ogre::Real rotation;
		int playerID;
		int type;
	};

	struct UnitsToUpdate
	{
		int id = -1;
		Ogre::Vector3 position;
		Ogre::Real rotation;
		int playerID;
		int type;
	};

	std::vector<UnitsToCreateData>* unitsToCreate;
	std::mutex unitsToCreateLock;

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

	std::mutex unitsToUpdateLock;
	std::vector<UnitsToUpdate>* unitsToUpdate;

};

#endif