#ifndef _SERVER_H
#define _SERVER_H

#include "Unit.h"
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "Messages.h"
#include "Building.h"

class Game;

class Server
{
public:
	Server(Game* game, Ogre::SceneManager* sceneManager);
	~Server();

	void update(); // simulates the game
	void waitForMessages(SOCKET sock); // Dedicated to separate thread to recieve input from players
	void listenForConnections(); // Dedicated thread to waiting for connections

private:

	void sendUnitToClients(Unit* unit);

	void sendBuildingToClient(Building* building);

	void addUnit(Unit* unit);
	void addBuilding(Building* building);

	std::vector<SOCKET>* sockets;

	std::vector<Unit*>* units;
	std::mutex unitsLock;


	std::thread* messageRecievingThread;
	SOCKET sock;
	int numberOfPlayers;
	std::mutex playerLock;
	struct sockaddr_in connection;
	Game* game;
	Ogre::SceneManager* sceneManager;

	struct UnitPathFindingStruct
	{
		Unit* unit;
		Tile* destinationTile;
		Unit* targetEnemy;
	};

	std::list<UnitPathFindingStruct>* pathFindingQueue;
	std::mutex pathFindingLock;

	std::vector<Building*>* buildings;
	std::mutex buildingsLock;

};

#endif