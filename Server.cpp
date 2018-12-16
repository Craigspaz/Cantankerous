#include "Server.h"
#include "Game.h"
#include "Tank.h"
#include <thread>
#include <chrono>

Server::Server(Game* game, Ogre::SceneManager* sceneManager)
{
	this->game = game;
	this->sceneManager = sceneManager;

	//Create test tank
	units = new std::vector<Unit*>();
	sockets = new std::vector<SOCKET>();
	pathFindingQueue = new std::list<UnitPathFindingStruct>();
	buildings = new std::vector<Building*>();
	numberOfPlayers = 1;

	sock = Messages::createSocket();
	connection.sin_family = AF_INET;
	connection.sin_addr.s_addr = INADDR_ANY;
	connection.sin_port = htons(1234);
	Messages::bindSocket(sock,connection);
	messageRecievingThread = new std::thread(&Server::listenForConnections,this);
	messageRecievingThread->detach();

	//std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	Tank* tank = new Tank(Ogre::Vector3(0, 10, 0), this->sceneManager, 1);
	addUnit(tank);
	Tank* tank1 = new Tank(Ogre::Vector3(300, 10, 0), this->sceneManager, 2);
	addUnit(tank1);
	Building* building = new Building(Ogre::Vector3(0, 20, 0), this->sceneManager, 1, BUILDING_CONSTRUCTOR);
	addBuilding(building);
	
}


Server::~Server()
{
}


void Server::update()
{
	unitsLock.lock();
	for (auto unit : *units)
	{
		//unit->setPosition(unit->getPosition() + Ogre::Vector3(0.05, 0, 0));
		unit->update(game->getCurrentLevel());

		// tmp
		//if (unit->isMoving() == false && unit->getPosition().x == 0 && unit->getPosition().z == 0)
		//{
		//	std::cout << "Finding path to : " << (game->getCurrentLevel()->getTiles()->at(170)->getGridPosition()) << std::endl;
		//	unit->setDestination(game->getCurrentLevel()->getTiles()->at(170), game->getCurrentLevel());
		//}

		//std::cout << "Unit Position: " << unit->getPosition() << std::endl;
		this->sendUnitToClients(unit);
	}
	unitsLock.unlock();

	pathFindingLock.lock();
	for (auto a : *pathFindingQueue)
	{
		a.unit->setDestination(a.destinationTile, this->game->getCurrentLevel());
	}
	pathFindingQueue->clear();
	pathFindingLock.unlock();

	buildingsLock.lock();
	for (auto building : *buildings)
	{
		int IDRet = building->update();
		if (IDRet >= 0)
		{
			if (IDRet == UNIT_TANK)
			{
				std::cout << "Creating tank..." << std::endl;
				Tank* tank = new Tank(Ogre::Vector3(building->getPosition().x, 10, building->getPosition().z), this->sceneManager, 1);
				tank->update(game->getCurrentLevel());
				tank->setDestination(game->getCurrentLevel()->getTiles()->at(170), game->getCurrentLevel());
				addUnit(tank);
			}
		}
		this->sendBuildingToClient(building);
	}
	buildingsLock.unlock();

}

void Server::listenForConnections()
{
	while (true)
	{
		Messages::listenForConnections(sock);
		SOCKET newSocket = Messages::acceptConnection(sock);
		sockets->push_back(newSocket);

		std::thread* thread = new std::thread(&Server::waitForMessages, this, newSocket);
		thread->detach();
	}
}

void Server::waitForMessages(SOCKET sock)
{
	while (true)
	{
		char buffer[1024];
		//printf("Receiving message from client\n");
		int bytesReceived = Messages::receiveMessage(sock, buffer, 1024);
		if (bytesReceived == 0)
		{
			continue;
		}
		if (bytesReceived == 3 && buffer[0] == 0x09 && buffer[1] == 0x00 && buffer[2] == 0x00)
		{
			char sendBuffer[1024];
			playerLock.lock();
			std::string filename = "<Level>" + game->getCurrentLevelFileName() + "</Level><PlayerID>" + std::to_string(numberOfPlayers + 1) + "</PlayerID>";
			playerLock.unlock();
			sendBuffer[0] = 0xFF;
			sendBuffer[1] = filename.length() & 0xFF00;
			sendBuffer[2] = filename.length() & 0x00FF;
			char* tmpSendBuffer = sendBuffer + 3;
			strncpy(tmpSendBuffer, filename.c_str(), 255);
			//printf("Sending message to client\n");
			int bytesSent = Messages::sendMessage(sock, sendBuffer, 1024);
			if (bytesSent == 0)
			{
				return;
			}
		}
		else if (buffer[0] == 0x03)
		{
			//std::cout << "Received pathfinding request message" << std::endl;
			buffer[bytesReceived] = '\0';
			printf("Receiving unit add message\n");
			char* tmpStart = buffer + 3;
			//std::cout << std::endl << "Received message: " << std::endl << tmpStart << std::endl;
			bool inID = false;
			bool inGridCoords = false;
			bool inGridCoordsX = false;
			bool inGridCoordsY = false;

			int id = -1;
			Ogre::Vector2 gridCoords(0, 0);

			char* tmpStart1 = buffer + 3;
			char* token = strtok(tmpStart1, ">");
			while (token != NULL)
			{
				std::string tmp = token;
				bool setFlag = false;
				if (tmp == "<UnitID")
				{
					inID = true;
					setFlag = true;
				}
				else if (tmp == "<GridCoords")
				{
					inGridCoords = true;
					setFlag = true;
				}
				else if (tmp == "<X" && inGridCoords)
				{
					inGridCoordsX = true;
					setFlag = true;
				}
				else if (tmp == "<Y" && inGridCoords)
				{
					inGridCoordsY = true;
					setFlag = true;
				}

				if (!setFlag)
				{
					if (inID)
					{
						id = std::atoi(tmp.substr(0, tmp.find("</ID")).c_str());
						inID = false;
					}
					else if (inGridCoordsX)
					{
						gridCoords.x = std::atoi(tmp.substr(0, tmp.find("</X")).c_str());
						inGridCoordsX = false;
					}
					else if (inGridCoordsY)
					{
						gridCoords.y = std::atoi(tmp.substr(0, tmp.find("</Y")).c_str());
						inGridCoordsY = false;
						inGridCoords = false;
					}
				}
				token = strtok(NULL, ">");
			}
			Unit* selectedUnit = NULL;
			unitsLock.lock();
			for (auto unit : *units)
			{
				if (unit->getUnitID() == id)
				{
					selectedUnit = unit;
					break;
				}
			}
			unitsLock.unlock();
			Tile* endTile = NULL;
			for (std::vector<Tile*>::iterator i = this->game->getCurrentLevel()->getTiles()->begin(); i != this->game->getCurrentLevel()->getTiles()->end(); i++)
			{
				if ((*i)->getGridPosition().x == gridCoords.x && (*i)->getGridPosition().y == gridCoords.y)
				{
					endTile = *i;
					break;
				}
			}
			UnitPathFindingStruct data;
			data.unit = selectedUnit;
			data.destinationTile = endTile;
			pathFindingLock.lock();
			pathFindingQueue->push_back(data);
			pathFindingLock.unlock();
		}
		else if (buffer[0] == 0x04)
		{
			buffer[bytesReceived] = '\0';
			char* tmpStart = buffer + 3;
			char* token = strtok(tmpStart, ">");

			bool inID = false;
			bool inToQueue = false;

			int id = -1;
			int queueID = -1;
			while (token != NULL)
			{
				std::string tmp = token;
				bool setFlag = false;
				if (tmp == "<ID")
				{
					inID = true;
					setFlag = true;
				}
				else if (tmp == "<ToQueue")
				{
					inToQueue = true;
					setFlag = true;
				}

				if (!setFlag)
				{
					if (inID)
					{
						id = std::atoi(tmp.substr(0, tmp.find("</ID")).c_str());
						inID = false;
					}
					else if (inToQueue)
					{
						queueID = std::atoi(tmp.substr(0, tmp.find("</ToQueue")).c_str());
						inToQueue = false;
					}
				}
				token = strtok(NULL, ">");
			}

			if (queueID == -1)
			{
				continue;
			}
			std::cout << "Adding unit: " << queueID << " to the queue" << std::endl;
			buildingsLock.lock();
			for (auto building : *buildings)
			{
				if (building->getID() == id)
				{
					building->addUnitToQueue(queueID);
					break;
				}
			}
			buildingsLock.unlock();
		}
	}
}

void Server::sendUnitToClients(Unit* unit)
{
	unit->lock();
	//printf("Sending unit to clients");
	char sendBuffer[1024];
	Ogre::Vector3 facingDirection = unit->getDirectionMoving();
	sendBuffer[0] = 0x01;
	std::string message = "<ID>" + std::to_string(unit->getUnitID());
	message += "</ID><Position><X>";
	message += std::to_string(unit->getPosition().x);
	message += "</X><Y>";
	message += std::to_string(unit->getPosition().y);
	message += "</Y><Z>";
	message += std::to_string(unit->getPosition().z);
	message += "</Z></Position><Direction><X>";
	message += std::to_string(facingDirection.x);
	message += "</X><Y>";
	message += std::to_string(facingDirection.y);
	message += "</Y><Z>";
	message += std::to_string(facingDirection.z);
	message += "</Z></Direction><Scale><X>";
	message += std::to_string(unit->getScale().x);
	message += "</X><Y>";
	message += std::to_string(unit->getScale().y);
	message += "</Y><Z>";
	message += std::to_string(unit->getScale().z);
	message += "</Z></Scale>";
	message += "<PlayerID>";
	message += std::to_string(unit->getPlayerControlledBy());
	message += "</PlayerID><Type>";
	message += std::to_string(unit->getType());
	message += "</Type>";
	unit->unlock();
	unsigned short length = message.length();
	sendBuffer[1] = length & 0xFF00;
	sendBuffer[2] = length & 0x00FF;
	//std::cout << "Length of message to send: " << length << std::endl;
	short i = 0;
	for (i = 0; i < length; i++)
	{
		sendBuffer[i + 3] = message.at(i);
	}
	for (auto s : *sockets)
	{
		Messages::sendMessage(s, sendBuffer, length + 3);
	}
	//printf("Sent unit add message");
}


void Server::addUnit(Unit* unit)
{
	unitsLock.lock();
	this->units->push_back(unit);
	unitsLock.unlock();
	sendUnitToClients(unit);
}

void Server::addBuilding(Building* building)
{
	buildingsLock.lock();
	this->buildings->push_back(building);
	buildingsLock.unlock();
	this->sendBuildingToClient(building);
}


void Server::sendBuildingToClient(Building* building)
{
	building->lock();
	//std::cout << "Sending building to clients: " << building->getPosition() << std::endl;
	char sendBuffer[1024];
	sendBuffer[0] = 0x02;
	std::string message = "<ID>" + std::to_string(building->getID());
	message += "</ID><Position><X>";
	message += std::to_string(building->getPosition().x);
	message += "</X><Y>";
	message += std::to_string(building->getPosition().y);
	message += "</Y><Z>";
	message += std::to_string(building->getPosition().z);
	message += "</Z></Position><PlayerID>";
	message += std::to_string(building->getControllingPlayerID());
	message += "</PlayerID><Type>";
	message += std::to_string(building->getType());
	message += "</Type><Queue>";
	for (int j = 0; j < building->getCurrentSizeOfQueue(); j++)
	{
		message += "<Item>";
		message += std::to_string(building->getQueue()[j]);
		message += "</Item>";
	}
	message += "</Queue>";
	building->unlock();

	unsigned short length = message.length();
	//std::cout << "Length of message to send: " << length << std::endl;
	sendBuffer[1] = length & 0xFF00;
	sendBuffer[2] = length & 0x00FF;
	short i = 0;
	for (i = 0; i < length; i++)
	{
		sendBuffer[i + 3] = message.at(i);
	}
	//std::cout << "Building position: " << building->getPosition() << std::endl;
	for (auto s : *sockets)
	{
		Messages::sendMessage(s, sendBuffer, length + 3);
	}
}