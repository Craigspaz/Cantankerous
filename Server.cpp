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
	projectiles = new std::vector<Projectile*>();
	numberOfPlayers = 1;

	sock = Messages::createSocket();
	connection.sin_family = AF_INET;
	connection.sin_addr.s_addr = INADDR_ANY;
	connection.sin_port = htons(1234);
	Messages::bindSocket(sock,connection);
	messageRecievingThread = new std::thread(&Server::listenForConnections,this);
	messageRecievingThread->detach();

	//std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	Building* building = new Building(Ogre::Vector3(0, 20, 0), this->sceneManager, 1, BUILDING_CONSTRUCTOR);
	addBuilding(building);

	Building* building2 = new Building(Ogre::Vector3(1800, 20, 100), this->sceneManager, 2, BUILDING_CONSTRUCTOR);
	addBuilding(building2);
	
}


Server::~Server()
{
	for (auto unit : *units)
	{
		delete unit;
	}
	delete units;
	delete sockets;
	delete pathFindingQueue;
	for (auto building : *buildings)
	{
		delete building;
	}
	delete buildings;
	for (auto projectile : *projectiles)
	{
		delete projectile;
	}
	delete projectiles;
}


void Server::update()
{
	unitsLock.lock();
	int unitIndex = 0;
	while (true)
	{
		bool madeItAllTheWayThrough = true;
		int counter = 0;
		for (auto iterator = units->begin(); iterator != units->end(); iterator++)
		{
			if (counter != unitIndex)
			{
				continue;
			}
			if ((*iterator)->isDead())
			{
				this->sendUnitToClients(*iterator);
				delete *iterator;
				iterator = units->erase(iterator);
				madeItAllTheWayThrough = false;
				break;
			}
			else
			{
				projectilesLock.lock(); // just in case the unit decides to create a projectile
				(*iterator)->update(game->getCurrentLevel(), projectiles);
				projectilesLock.unlock();

				this->sendUnitToClients(*iterator);
			}
			unitIndex++;
			counter++;
		}
		if (madeItAllTheWayThrough)
		{
			break;
		}
	}
	unitsLock.unlock();

	pathFindingLock.lock();
	for (auto a : *pathFindingQueue)
	{
		a.unit->setDestination(a.destinationTile, this->game->getCurrentLevel());
		if (a.targetEnemy != NULL)
		{
			a.unit->setTarget(a.targetEnemy);
		}
		else if (a.targetEnemyBuilding != NULL)
		{
			a.unit->setTarget(a.targetEnemyBuilding, a.destinationTile);
		}
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
				Tank* tank = new Tank(Ogre::Vector3(building->getPosition().x, 10, building->getPosition().z), this->sceneManager, building->getControllingPlayerID());
				tank->update(game->getCurrentLevel(), projectiles);
				addUnit(tank);
			}
		}
		this->sendBuildingToClient(building);
	}
	buildingsLock.unlock();

	projectilesLock.lock();
	int projectileIndex = 0;
	while (true)
	{
		bool madeItAllTheWayThrough = true;
		int counter = 0;
		for (auto iterator = projectiles->begin(); iterator != projectiles->end(); iterator++)
		{
			if (counter != projectileIndex)
			{
				continue;
			}
			if ((*iterator)->isDestroyed())
			{
				delete (*iterator);
				iterator = projectiles->erase(iterator);
				madeItAllTheWayThrough = false;
				break;
			}
			else
			{
				(*iterator)->update();
				sendProjectileToClient(*iterator);
			}
			projectileIndex++;
			counter++;
		}
		if (madeItAllTheWayThrough)
		{
			break;
		}
	}
	projectilesLock.unlock();

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
			std::string filename = "<Level>" + game->getCurrentLevelFileName() + "</Level><PlayerID>" + std::to_string(numberOfPlayers++) + "</PlayerID>";
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
			data.targetEnemy = NULL;
			data.targetEnemyBuilding = NULL;
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
		else if (buffer[0] == 0x05) // Receive pathfinding message with lock on to target
		{
			//std::cout << "Received pathfinding request message" << std::endl;
			buffer[bytesReceived] = '\0';
			printf("Receiving unit add message\n");
			char* tmpStart = buffer + 3;
			//std::cout << std::endl << "Received message: " << std::endl << tmpStart << std::endl;
			bool inID = false;
			bool inEnemyID = false;

			int id = -1;
			int enemyID = -1;

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
				else if (tmp == "<EnemyID")
				{
					inEnemyID = true;
					setFlag = true;
				}

				if (!setFlag)
				{
					if (inID)
					{
						id = std::atoi(tmp.substr(0, tmp.find("</ID")).c_str());
						inID = false;
					}
					else if (inEnemyID)
					{
						enemyID = std::atoi(tmp.substr(0, tmp.find("</EnemyID")).c_str());
						inEnemyID = false;
					}
				}
				token = strtok(NULL, ">");
			}
			Unit* selectedUnit = NULL;
			Unit* targetEnemy = NULL;
			unitsLock.lock();
			for (auto unit : *units)
			{
				if (unit->getUnitID() == id)
				{
					selectedUnit = unit;
				}
				if (unit->getUnitID() == enemyID)
				{
					targetEnemy = unit;
				}
			}
			unitsLock.unlock();
			
			UnitPathFindingStruct data;
			data.unit = selectedUnit;
			data.destinationTile = targetEnemy->getCurrentTile();
			data.targetEnemy = targetEnemy;
			data.targetEnemyBuilding = NULL;
			pathFindingLock.lock();
			pathFindingQueue->push_back(data);
			pathFindingLock.unlock();
		}
		else if (buffer[0] == 0x07)
		{
			//std::cout << "Received pathfinding request message" << std::endl;
			buffer[bytesReceived] = '\0';
			printf("Receiving unit add message\n");
			char* tmpStart = buffer + 3;
			//std::cout << std::endl << "Received message: " << std::endl << tmpStart << std::endl;
			bool inID = false;
			bool inEnemyID = false;

			int id = -1;
			int enemyID = -1;

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
				else if (tmp == "<EnemyBuildingID")
				{
					inEnemyID = true;
					setFlag = true;
				}

				if (!setFlag)
				{
					if (inID)
					{
						id = std::atoi(tmp.substr(0, tmp.find("</ID")).c_str());
						inID = false;
					}
					else if (inEnemyID)
					{
						enemyID = std::atoi(tmp.substr(0, tmp.find("</EnemyBuildingID")).c_str());
						inEnemyID = false;
					}
				}
				token = strtok(NULL, ">");
			}
			Unit* selectedUnit = NULL;
			Building* targetEnemyBuilding = NULL;
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
			
			buildingsLock.lock();
			for (auto building : *buildings)
			{
				if (building->getID() == id)
				{
					targetEnemyBuilding = building;
					break;
				}
			}
			buildingsLock.unlock();

			Tile* currentTile = NULL;
			for (auto tile : *(game->getCurrentLevel()->getTiles()))
			{
				Ogre::Vector3 currentPosition = targetEnemyBuilding->getPosition();
				Ogre::Vector3 tilesPosition = tile->getPosition();
				if (currentPosition.x > tilesPosition.x - (tile->getScale() / 2) && currentPosition.x < tilesPosition.x + (tile->getScale() / 2)) // if x
				{
					if (currentPosition.z > tilesPosition.z - (tile->getScale() / 2) && currentPosition.z < tilesPosition.z + (tile->getScale() / 2)) // if y
					{
						currentTile = tile;
						break;
					}
				}
			}

			UnitPathFindingStruct data;
			data.unit = selectedUnit;
			data.destinationTile = currentTile;
			data.targetEnemy = NULL;
			data.targetEnemyBuilding = targetEnemyBuilding;
			pathFindingLock.lock();
			pathFindingQueue->push_back(data);
			pathFindingLock.unlock();
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
	message += "<Alive>";
	message += std::to_string(!unit->isDead());
	message += "</Alive>";
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


void Server::sendProjectileToClient(Projectile* projectile)
{
	//std::cout << "Sending building to clients: " << building->getPosition() << std::endl;
	char sendBuffer[1024];
	sendBuffer[0] = 0x06;
	std::string message = "<ID>" + std::to_string(projectile->getID());
	message += "</ID><Position><X>";
	message += std::to_string(projectile->getPosition().x);
	message += "</X><Y>";
	message += std::to_string(projectile->getPosition().y);
	message += "</Y><Z>";
	message += std::to_string(projectile->getPosition().z);
	message += "</Z></Position><PlayerID>";
	message += std::to_string(projectile->getControllingPlayer());
	message += "</PlayerID>";
	message += "<Alive>";;
	message += std::to_string(!projectile->isDestroyed());
	message += "</Alive>";

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