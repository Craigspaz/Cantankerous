#ifndef _SERVER_H
#define _SERVER_H

#include "Unit.h"
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "Messages.h"

class Game;

class Server
{
public:
	Server(Game* game, Ogre::SceneManager* sceneManager);
	~Server();

	void update(); // simulates the game

	void waitForMessages(); // Dedicated to separate thread to recieve input from players

private:
	void recieveMessage(); // helper function
	void sendMessage(); // helper function


	std::vector<Unit*> units;
	std::thread* messageRecievingThread;
	std::mutex unitsLock;
	SOCKET sock;
	struct sockaddr_in connection;
	Game* game;
	Ogre::SceneManager* sceneManager;
};

#endif