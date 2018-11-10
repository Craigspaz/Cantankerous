#ifndef _CLIENT_H
#define _CLIENT_H

#include <Ogre.h>
#include <iostream>
#include <string>
#include "Messages.h"
#include "Tile.h"

class Game;

class Client
{
public:
	Client(std::string ip, int port, Game* game, Ogre::SceneManager* sceneManager);
	~Client();

private:

	void processInitialMessage(char* message);
	void getInitialInfo();

	SOCKET sock;
	struct sockaddr_in connection;
	Game* game;
	Ogre::SceneManager* sceneManager;
	std::thread* messageRecievingThread;

};

#endif