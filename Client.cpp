#include "Client.h"
#include <stdio.h>
#include <stdlib.h>
#include "Game.h"

Client::Client(std::string ip, int port, Game* game, Ogre::SceneManager* sceneManager)
{
	localCopyOfUnits = new std::vector<Unit*>();
	unitsToCreate = new std::vector<UnitsToCreateData>();
	unitsToUpdate = new std::vector<UnitsToUpdate>();
	this->sceneManager = sceneManager;
	this->game = game;
	this->selectedUnit = NULL;
	sock = Messages::createSocket();
	inet_pton(AF_INET, ip.c_str(), &(connection.sin_addr.s_addr));
	connection.sin_family = AF_INET;
	connection.sin_port = htons(port);
	if (Messages::connectToServer(sock, connection) != 0)
	{
		printf("Failed to initialze connection to server. Closing game. Please try again later\n");
		exit(-1);
	}

	// Ask server for terrain
	char buffer[1024];
	buffer[0] = 0x09;
	buffer[1] = 0x00;
	buffer[2] = 0x00;
	int bytesSent = Messages::sendMessage(sock, buffer, 3);
	if (bytesSent != 3)
	{
		printf("Failed to send message!");
	}
	//printf("Sent message to server for basic info\n");

	//messageRecievingThread = new std::thread(&Client::getInitialInfo, this);
	//messageRecievingThread->detach();
	getInitialInfo();
}

void Client::getInitialInfo()
{
	char buffer[1024];
	char* tmpHeapBuffer = (char*)malloc(1024 * sizeof(char));
	int currentSize = 0;
	int realSize = 0;
	if (tmpHeapBuffer == NULL)
	{
		printf("Out of memory\n");
		exit(-1);
	}
	int bytesReceived = 0;
	//do
	//{
		printf("Attempting to receive basic info\n");
		bytesReceived = Messages::receiveMessage(sock, buffer, 1024);
		if (currentSize == 0)
		{
			strncpy(tmpHeapBuffer, buffer + 3, 1024);
		}
		else
		{
			tmpHeapBuffer = (char*)realloc(tmpHeapBuffer, (currentSize + 1024) * sizeof(char));
			strncat(tmpHeapBuffer, buffer, 1024);
		}
		currentSize += 1024;
		realSize += bytesReceived;

	//} while (bytesReceived != 0);

	processInitialMessage(tmpHeapBuffer);
	free(tmpHeapBuffer);

	messageRecievingThread = new std::thread(&Client::receiveMessages,this);
	messageRecievingThread->detach();
}

void Client::processInitialMessage(char* message)
{
	char* token = strtok(message, ">");

	bool foundLevelTag = false;
	while (token != NULL)
	{
		std::string tmp = token;
		if (tmp == "<Level")
		{
			foundLevelTag = true;
		}
		else if (foundLevelTag)
		{
			std::string path = __FILE__; //gets the current cpp file's path with the cpp file
			path = path.substr(0, 1 + path.find_last_of('\\')); //removes filename to leave path
			std::string fileName = tmp.substr(0, tmp.find("</Level"));
			game->setLevel(new Level(path, fileName, sceneManager));
			foundLevelTag = false;
		}

		token = strtok(NULL, ">");
	}
}

Client::~Client()
{

}

Unit* Client::checkIfRayIntersectsWithUnits(Ogre::Ray ray)
{
	//TMP local test
	unitsLock.lock();
	for (auto unit : *localCopyOfUnits)
	{
		if (unit->getType() == UNIT_TANK)
		{
			Tank* tank = (Tank*)unit;
			std::pair<bool, Ogre::Real> col = ray.intersects(tank->getBaseEntity()->getBoundingBox());
			if (col.first)
			{
				unitsLock.unlock();
				return unit;
			}
			std::pair<bool, Ogre::Real> col2 = ray.intersects(tank->getTurretEntity()->getBoundingBox());
			if (col2.first)
			{
				unitsLock.unlock();
				return unit;
			}
		}
	}
	unitsLock.unlock();
	return NULL;
}

void Client::handleClick(Camera* camera, Ogre::Vector3 cameraPosition, OgreBites::MouseButtonEvent event, Ogre::Vector3 direction)
{
	if (event.type == MOUSEBUTTONDOWN)
	{
		if (event.button == BUTTON_LEFT)
		{			
			double x = event.x / (double)game->getRenderWindow()->getWidth();
			double y = event.y / (double)game->getRenderWindow()->getHeight();
			Ray ray = camera->getCameraToViewportRay(x, y);
			Ogre::RaySceneQuery* query = this->sceneManager->createRayQuery(Ogre::Ray());
			if (query == NULL)
			{
				exit(-1);
			}
			query->setSortByDistance(true); 
			Ogre::Ray theRay((camera->getDerivedPosition() + ray.getDirection() * 100.0) + Ogre::Vector3(x, 0, y), ray.getDirection());
			query->setRay(theRay);
			Ogre::RaySceneQueryResult& result = query->execute();
			for (Ogre::RaySceneQueryResult::iterator i = result.begin(); i != result.end(); i++)
			{
				if ((*i).distance > 2.0f)
				{
					//std::cout << "Found collision" << std::endl;
					Ogre::Entity* res = (Ogre::Entity*)((*i).movable);
					if (res != NULL)
					{
						unitsLock.lock();
						for (auto unit : *localCopyOfUnits)
						{
							if (unit->getType() == UNIT_TANK)
							{
								Tank* tank = (Tank*)unit;
								if (res == tank->getBaseEntity() || res == tank->getTurretEntity())
								{
									//std::cout << "Clicked on tank" << std::endl;
									if (selectedUnit != NULL)
									{
										selectedUnit->setSelected(false);
									}
									tank->setSelected(true);
									selectedUnit = unit;
									unitsLock.unlock();
									return;
								}
							}
						}
						unitsLock.unlock();
						if (selectedUnit != NULL)
						{
							Tile* endTile = NULL;
							for (std::vector<Tile*>::iterator j = this->game->getCurrentLevel()->getTiles()->begin(); j != this->game->getCurrentLevel()->getTiles()->end(); j++)
							{
								if ((*j)->getEntity() == res)
								{
									endTile = *j;
									break;
								}
							}
							if (endTile != NULL)
							{
								tellServerToDeterminePath(selectedUnit->getUnitID(), endTile->getGridPosition());
							}
						}
					}
				}
			}
		}
	}
}

void Client::receiveMessages()
{
	while (true)
	{
		char buffer[4097];
		int bytesReceived = Messages::receiveMessage(sock, buffer, 4096);
		if (bytesReceived == 0)
		{
			return;
		}
		if (buffer[0] == 0x01) // Unit added message
		{
			buffer[bytesReceived] = '\0';
			printf("Receiving unit add message\n");
			char* tmpStart = buffer + 3;
			//std::cout << std::endl << "Received message: " << std::endl << tmpStart << std::endl;
			bool inID = false;
			bool inPosition = false;
			bool inPositionX = false;
			bool inPositionY = false;
			bool inPositionZ = false;
			bool inRotation = false;
			bool inScale = false;
			bool inScaleX = false;
			bool inScaleY = false;
			bool inScaleZ = false;
			bool inPlayerID = false;
			bool inType = false;
			bool inDestination = false;
			bool inDestinationX = false;
			bool inDestinationY = false;
			bool inDestinationZ = false;

			int id = -1;
			Ogre::Vector3 position(0,0,0);
			Ogre::Real rotation = 0;
			Ogre::Vector3 scale(0,0,0);
			Ogre::Vector3 destination(0,0,0);
			int playerID = 0;
			int type = UNIT_TANK;

			char* tmpStart1 = buffer + 3;
			char* token = strtok(tmpStart1, ">");
			while (token != NULL)
			{
				std::string tmp = token;
				bool setFlag = false;
				if (tmp == "<ID")
				{
					inID = true;
					setFlag = true;
				}
				else if (tmp == "<Position")
				{
					inPosition = true;
					setFlag = true;
				}
				else if (tmp == "<X" && inPosition)
				{
					inPositionX = true;
					setFlag = true;
				}
				else if (tmp == "<Y" && inPosition)
				{
					inPositionY = true;
					setFlag = true;
				}
				else if (tmp == "<Z" && inPosition)
				{
					inPositionZ = true;
					setFlag = true;
				}
				else if (tmp == "<Rotation")
				{
					inRotation = true;
					setFlag = true;
				}
				else if (tmp == "<Scale")
				{
					inScale = true;
					setFlag = true;
				}
				else if (tmp == "<X" && inScale)
				{
					inScaleX = true;
					setFlag = true;
				}
				else if (tmp == "<Y" && inScale)
				{
					inScaleY = true;
					setFlag = true;
				}
				else if (tmp == "<Z" && inScale)
				{
					inScaleZ = true;
					setFlag = true;
				}
				else if (tmp == "<PlayerID")
				{
					inPlayerID = true;
					setFlag = true;
				}
				else if (tmp == "<Type")
				{
					inType = true;
					setFlag = true;
				}
				else if (tmp == "<Destination")
				{
					inDestination = true;
					setFlag = true;
				}
				else if (tmp == "<X" && inDestination)
				{
					inDestinationX = true;
					setFlag = true;
				}
				else if (tmp == "<Y" && inDestination)
				{
					inDestinationY = true;
					setFlag = true;
				}
				else if (tmp == "<Z" && inDestination)
				{
					inDestinationZ = true;
					setFlag = true;
				}

				if (!setFlag)
				{
					if (inID)
					{
						id = std::atoi(tmp.substr(0, tmp.find("</ID")).c_str());
						inID = false;
					}
					else if (inPositionX)
					{
						position.x = std::atof(tmp.substr(0, tmp.find("</X")).c_str());
						inPositionX = false;
					}
					else if (inPositionY)
					{
						position.y = std::atof(tmp.substr(0, tmp.find("</Y")).c_str());
						inPositionY = false;
					}
					else if (inPositionZ)
					{
						position.z = std::atof(tmp.substr(0, tmp.find("</Z")).c_str());
						inPositionZ = false;
						inPosition = false;
					}
					else if (inRotation)
					{
						rotation = std::atof(tmp.substr(0, tmp.find("</Rotation")).c_str());
						inRotation = false;
					}
					else if (inScaleX)
					{
						scale.x = std::atof(tmp.substr(0, tmp.find("</X")).c_str());
						inScaleX = false;
					}
					else if (inScaleX)
					{
						scale.y = std::atof(tmp.substr(0, tmp.find("</Y")).c_str());
						inScaleY = false;
					}
					else if (inScaleZ)
					{
						scale.z = std::atof(tmp.substr(0, tmp.find("</Z")).c_str());
						inScaleZ = false;
						inScale = false;
					}
					else if (inPlayerID)
					{
						playerID = std::atoi(tmp.substr(0, tmp.find("</PlayerID")).c_str());
						inPlayerID = false;
					}
					else if (inType)
					{
						type = std::atoi(tmp.substr(0, tmp.find("</Type")).c_str());
						inType = false;
					}
					else if (inDestinationX)
					{
						destination.x = std::atof(tmp.substr(0, tmp.find("</X")).c_str());
						inDestinationX = false;
					}
					else if (inDestinationY)
					{
						destination.y = std::atof(tmp.substr(0, tmp.find("</Y")).c_str());
						inDestinationY = false;
					}
					else if (inDestinationZ)
					{
						destination.z = std::atof(tmp.substr(0, tmp.find("</Z")).c_str());
						inDestinationZ = false;
						inDestination = false;
					}
				}
				token = strtok(NULL, ">");
			}
			//if (type == UNIT_TANK)
			{
				UnitsToCreateData data;
				data.id = id;
				data.playerID = playerID;
				data.position = position;
				data.rotation = rotation;
				data.type = type;

				unitsToCreateLock.lock();
				unitsToCreate->push_back(data);
				unitsToCreateLock.unlock();
				/*Tank* tank = new Tank(position, sceneManager, playerID, id);
				tank->setRotation(Ogre::Degree(rotation));
				unitsLock.lock();
				localCopyOfUnits->push_back(tank);
				unitsLock.unlock();*/
				printf("Received and processed unit add message\n");
			}
		}
		else if (buffer[0] == 0x02) // Unit added message
		{
			buffer[bytesReceived] = '\0';
			//printf("Receiving unit update message\n");
			char* tmpStart = buffer + 3;
			//std::cout << std::endl << "Received message: " << std::endl << tmpStart << std::endl;
			bool inID = false;
			bool inPosition = false;
			bool inPositionX = false;
			bool inPositionY = false;
			bool inPositionZ = false;
			bool inRotation = false;
			bool inScale = false;
			bool inScaleX = false;
			bool inScaleY = false;
			bool inScaleZ = false;
			bool inPlayerID = false;
			bool inType = false;
			bool inDestination = false;
			bool inDestinationX = false;
			bool inDestinationY = false;
			bool inDestinationZ = false;

			int id = -1;
			Ogre::Vector3 position(0, 0, 0);
			Ogre::Real rotation = 0;
			Ogre::Vector3 scale(0, 0, 0);
			Ogre::Vector3 destination(0, 0, 0);
			int playerID = 0;
			int type = UNIT_TANK;

			char* tmpStart1 = buffer + 3;
			char* token = strtok(tmpStart1, ">");
			while (token != NULL)
			{
				std::string tmp = token;
				bool setFlag = false;
				if (tmp == "<ID")
				{
					inID = true;
					setFlag = true;
				}
				else if (tmp == "<Position")
				{
					inPosition = true;
					setFlag = true;
				}
				else if (tmp == "<X" && inPosition)
				{
					inPositionX = true;
					setFlag = true;
				}
				else if (tmp == "<Y" && inPosition)
				{
					inPositionY = true;
					setFlag = true;
				}
				else if (tmp == "<Z" && inPosition)
				{
					inPositionZ = true;
					setFlag = true;
				}
				else if (tmp == "<Rotation")
				{
					inRotation = true;
					setFlag = true;
				}
				else if (tmp == "<Scale")
				{
					inScale = true;
					setFlag = true;
				}
				else if (tmp == "<X" && inScale)
				{
					inScaleX = true;
					setFlag = true;
				}
				else if (tmp == "<Y" && inScale)
				{
					inScaleY = true;
					setFlag = true;
				}
				else if (tmp == "<Z" && inScale)
				{
					inScaleZ = true;
					setFlag = true;
				}
				else if (tmp == "<PlayerID")
				{
					inPlayerID = true;
					setFlag = true;
				}
				else if (tmp == "<Type")
				{
					inType = true;
					setFlag = true;
				}
				else if (tmp == "<Destination")
				{
					inDestination = true;
					setFlag = true;
				}
				else if (tmp == "<X" && inDestination)
				{
					inDestinationX = true;
					setFlag = true;
				}
				else if (tmp == "<Y" && inDestination)
				{
					inDestinationY = true;
					setFlag = true;
				}
				else if (tmp == "<Z" && inDestination)
				{
					inDestinationZ = true;
					setFlag = true;
				}

				if (!setFlag)
				{
					if (inID)
					{
						id = std::atoi(tmp.substr(0, tmp.find("</ID")).c_str());
						inID = false;
					}
					else if (inPositionX)
					{
						position.x = std::atof(tmp.substr(0, tmp.find("</X")).c_str());
						inPositionX = false;
					}
					else if (inPositionY)
					{
						position.y = std::atof(tmp.substr(0, tmp.find("</Y")).c_str());
						inPositionY = false;
					}
					else if (inPositionZ)
					{
						position.z = std::atof(tmp.substr(0, tmp.find("</Z")).c_str());
						inPositionZ = false;
						inPosition = false;
					}
					else if (inRotation)
					{
						rotation = std::atof(tmp.substr(0, tmp.find("</Rotation")).c_str());
						inRotation = false;
					}
					else if (inScaleX)
					{
						scale.x = std::atof(tmp.substr(0, tmp.find("</X")).c_str());
						inScaleX = false;
					}
					else if (inScaleX)
					{
						scale.y = std::atof(tmp.substr(0, tmp.find("</Y")).c_str());
						inScaleY = false;
					}
					else if (inScaleZ)
					{
						scale.z = std::atof(tmp.substr(0, tmp.find("</Z")).c_str());
						inScaleZ = false;
						inScale = false;
					}
					else if (inPlayerID)
					{
						playerID = std::atoi(tmp.substr(0, tmp.find("</PlayerID")).c_str());
						inPlayerID = false;
					}
					else if (inType)
					{
						type = std::atoi(tmp.substr(0, tmp.find("</Type")).c_str());
						inType = false;
					}
					else if (inDestinationX)
					{
						destination.x = std::atof(tmp.substr(0, tmp.find("</X")).c_str());
						inDestinationX = false;
					}
					else if (inDestinationY)
					{
						destination.y = std::atof(tmp.substr(0, tmp.find("</Y")).c_str());
						inDestinationY = false;
					}
					else if (inDestinationZ)
					{
						destination.z = std::atof(tmp.substr(0, tmp.find("</Z")).c_str());
						inDestinationZ = false;
						inDestination = false;
					}
				}
				token = strtok(NULL, ">");
			}
			//if (type == UNIT_TANK)
			{
				UnitsToUpdate data;
				data.id = id;
				data.playerID = playerID;
				data.position = position;
				data.rotation = rotation;
				data.type = type;

				unitsToUpdateLock.lock();
				unitsToUpdate->push_back(data);
				unitsToUpdateLock.unlock();
				/*Tank* tank = new Tank(position, sceneManager, playerID, id);
				tank->setRotation(Ogre::Degree(rotation));
				unitsLock.lock();
				localCopyOfUnits->push_back(tank);
				unitsLock.unlock();*/
				//printf("Received and processed unit update message\n");
			}
		}
	}
}

void Client::update(Ogre::SceneNode* cameraNode, int clientMode)
{
	unitsToCreateLock.lock();
	for (auto unit : *unitsToCreate)
	{
		if (unit.type == UNIT_TANK)
		{
			Tank* tank = new Tank(unit.position, sceneManager, unit.playerID, unit.id);
			tank->setOrientation(Quaternion(Ogre::Radian(Ogre::Degree(unit.rotation)), Ogre::Vector3::UNIT_Y));
			if (clientMode == CLIENT_MODE_PASSIVE)
			{
				tank->setVisible(false);
			}
			unitsLock.lock();
			localCopyOfUnits->push_back(tank);
			unitsLock.unlock();
		}
	}
	unitsToCreate->clear();
	unitsToCreateLock.unlock();

	unitsToUpdateLock.lock();
	for (auto unit : *unitsToUpdate)
	{
		bool foundUnitToUpdate = false;
		unitsLock.lock();
		for (auto realUnit : *localCopyOfUnits)
		{
			if (realUnit->getUnitID() == unit.id)
			{
				realUnit->setPosition(unit.position);
				realUnit->setPlayerControlledBy(unit.playerID);
				realUnit->setOrientation(Quaternion(Ogre::Radian(Ogre::Degree(unit.rotation)), Ogre::Vector3::UNIT_Y));
				foundUnitToUpdate = true;
				break;
			}
		}
		if (foundUnitToUpdate == false)
		{
			if (unit.type == UNIT_TANK)
			{
				Tank* tank = new Tank(unit.position, sceneManager, unit.playerID, unit.id);
				tank->setOrientation(Quaternion(Ogre::Radian(Ogre::Degree(unit.rotation)), Ogre::Vector3::UNIT_Y));
				//tank->setRotation(Ogre::Degree(unit.rotation));
				if (clientMode == CLIENT_MODE_PASSIVE)
				{
					tank->setVisible(false);
				}
				localCopyOfUnits->push_back(tank);
			}
		}
		unitsLock.unlock();
	}
	unitsToUpdate->clear();
	unitsToUpdateLock.unlock();
	
	//if (clientMode == CLIENT_MODE_PASSIVE)
	{
		//unitsLock.lock();
		//for (auto unit : *localCopyOfUnits)
		//{
			//std::cout << "Unit: " << unit->getUnitID() << " is at: " << unit->getPosition() << std::endl;
			//unit->update(game->getCurrentLevel());
		//}
		//unitsLock.unlock();
	}
	//unitsToCreateLock.unlock();
}


void Client::tellServerToDeterminePath(int unitID, Ogre::Vector2 gridCoords)
{
	char sendBuffer[1024];
	std::string message = "<UnitID>" + std::to_string(unitID) + "</UnitID><GridCoords><X>" + std::to_string(gridCoords.x) + "</X><Y>" + std::to_string(gridCoords.y) + "</Y></GridCoords>";
	sendBuffer[0] = 0x03;
	sendBuffer[1] = message.length() & 0xFF00;
	sendBuffer[2] = message.length() & 0x00FF;
	char* tmpSendBuffer = sendBuffer + 3;
	strncpy(tmpSendBuffer, message.c_str(), 1021);
	int bytesSent = Messages::sendMessage(this->sock, sendBuffer, message.length() + 3);
	if (bytesSent == 0)
	{
		return;
	}
}