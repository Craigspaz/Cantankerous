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
	Client(std::string ip, int port, Game* game, Ogre::SceneManager* sceneManager, OgreBites::TrayManager* trayManager);
	~Client();
	void handleClick(Ogre::Camera* camera, Ogre::Vector3 cameraPosition, OgreBites::MouseButtonEvent event, Ogre::Vector3 direction);
	void update(Ogre::SceneNode* cameraNode, int clientMode);
	Unit* checkIfRayIntersectsWithUnits(Ogre::Ray);

	void addUnitCreationToQueue(int type);
	void tellClientUserAskedToQueueUnit(int type);

private:

	struct UnitsToUpdate
	{
		int id = -1;
		Ogre::Vector3 position;
		Ogre::Vector3 directionFacing;
		int playerID;
		int type;
	};

	struct BuildingsToUpdateData
	{
		int id = -1;
		Ogre::Vector3 position;
		int playerID;
		int type;
		std::vector<int> queue;
	};

	void processInitialMessage(char* message);
	void getInitialInfo();
	void receiveMessages();
	void tellServerToDeterminePath(int unitID, Ogre::Vector2 gridCoords);
	void tellServerToDeterminePathAndLockOnToTarget(int enemyUnitID, int unitID);

	SOCKET sock;
	struct sockaddr_in connection;
	Game* game;
	int playerID;
	Ogre::SceneManager* sceneManager;
	std::thread* messageRecievingThread;

	std::vector<Unit*>* localCopyOfUnits;
	std::mutex unitsLock;

	std::mutex unitsToUpdateLock;
	std::vector<UnitsToUpdate>* unitsToUpdate;

	Unit* selectedUnit;

	std::vector<Building*>* localCopyOfBuildings;
	std::mutex buildingsLock;

	Building* selectedBuilding;

	std::vector<BuildingsToUpdateData>* buildingsToUpdate;
	std::mutex buildingsToUpdateLock;

	OgreBites::TrayManager* trayManager;

	std::mutex selectedUnitLock;
	std::mutex selectedBuildingLock;


	std::vector<Projectile*>* localCopyOfProjectiles;
	std::mutex projectilesLock;

};

#endif