#ifndef _SERVER_H
#define _SERVER_H

#include "Unit.h"
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

class Server
{
public:
	Server();
	~Server();

	void update(); // simulates the game

	void waitForMessages(); // Dedicated to separate thread to recieve input from players

private:
	void recieveMessage(); // helper function
	void sendMessage(); // helper function


	std::vector<Unit*> units;
	std::thread* messageRecievingThread;
	std::mutex unitsLock;
};

#endif