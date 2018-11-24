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
	for (auto unit : *localCopyOfUnits)
	{
		if (unit->getType() == UNIT_TANK)
		{
			Tank* tank = (Tank*)unit;
			std::pair<bool, Ogre::Real> col = ray.intersects(tank->getBaseEntity()->getBoundingBox());
			if (col.first)
			{
				return unit;
			}
			std::pair<bool, Ogre::Real> col2 = ray.intersects(tank->getTurretEntity()->getBoundingBox());
			if (col2.first)
			{
				return unit;
			}
		}
	}
	return NULL;
}

void Client::handleClick(Camera* camera, Ogre::Vector3 cameraPosition, OgreBites::MouseButtonEvent event, Ogre::Vector3 direction)
{
	if (event.type == MOUSEBUTTONDOWN)
	{
		if (event.button == BUTTON_LEFT)
		{
			printf("Left click\n");
			// Get world position of click
			//std::cout << "Clicked at : " << (Ogre::Vector2(cameraPosition.x, cameraPosition.z) + Ogre::Vector2(event.x, event.y)) << " " << (Ogre::Vector2(cameraPosition.x, cameraPosition.y)) << " " << Ogre::Vector2(event.x, event.y) << std::endl;

			int width = game->getRenderWindow()->getWidth();
			int height = game->getRenderWindow()->getHeight();
			int nX = event.x - (width / 2);
			int nY = -1.0 * (event.y - (height / 2));
			Ogre::Ray r = game->getRenderWindow()->getViewport(0)->getCamera()->getCameraToViewportRay(nX, nY);
			std::cout << "Ray start: " << r.getOrigin() << std::endl;
			//std::cout << "Start Position : " << cameraPosition + Ogre::Vector3(nX, nY * cos(40.0 * 3.14 / 180.0), 0) << std::endl;
			//Ogre::Ray mouseRay(cameraPosition + Ogre::Vector3(0, nY * cos(40.0 * 3.14 / 180.0), 0), direction);
			
			double x = event.x / (double)game->getRenderWindow()->getWidth();
			double y = event.y / (double)game->getRenderWindow()->getHeight();
			Ray ray = camera->getCameraToViewportRay(x, y);
			SceneNode* testNode = this->sceneManager->getRootSceneNode()->createChildSceneNode();
			testNode->setPosition((camera->getDerivedPosition() + ray.getDirection() * 225.0) + Ogre::Vector3(x, 0, y));
			testNode->setScale(testNode->getScale() / 10.0);
			testNode->setScale(testNode->getScale().x, testNode->getScale().y, 10.0);
			//testNode->rotate(Ogre::Vector3::UNIT_X, Ogre::Radian(Ogre::Degree(direction.angleBetween(Ogre::Vector3::UNIT_Z)) + Ogre::Degree(50)));
			testNode->setOrientation(Ogre::Vector3::UNIT_Z.getRotationTo(ray.getDirection()));
			Ogre::Entity* entity = this->sceneManager->createEntity(Ogre::SceneManager::PT_CUBE);
			testNode->attachObject(entity);



			/*Ray testRay(testNode->getPosition(), direction);
						
			Unit* unit = checkIfRayIntersectsWithUnits(testRay);
			if (unit == NULL)
			{
				std::cout << "Not colliding with unit" << std::endl;
				unitsLock.lock();
				for (auto u : *localCopyOfUnits)
				{
					std::cout << std::endl << "Unit: " << u->getPosition() << std::endl;
				}
				unitsLock.unlock();
			}
			else
			{
				std::cout << "Ray intersected with unit with ID: " << unit->getUnitID() << std::endl;
			}*/
			/*int width = game->getRenderWindow()->getWidth();
			int height = game->getRenderWindow()->getHeight();
			std::cout << "Width: " << width << " Height: " << height << std::endl;

			int nX = event.x - (width / 2);
			int nY = event.y - (height / 2);
			Ogre::Vector3 rayStart = Ogre::Vector3(cameraPosition.x, cameraPosition.y, cameraPosition.z);

			int distanceFromCenterOfCameraY = height - event.y;
			if (event.y > height / 2)
			{
				distanceFromCenterOfCameraY *= -1;
			}

			double distanceFromClick = abs(sqrt(pow((height / 2 + distanceFromCenterOfCameraY),2) + pow((height / 2 + distanceFromCenterOfCameraY) * sin(40.0 * 3.14 / 180.0), 2)));
			//double distanceFromClickZ = 100.0 * tan(40.0 * 3.14 / 180.0);
			rayStart.x = rayStart.x / (200.0 / cos(40.0 * 3.14 / 180.0));
			rayStart.z = rayStart.z - distanceFromClick;
			std::cout << "Clicked at: " << rayStart << " " << direction << std::endl;

			//Ogre::Vector3 vectorFrom = Ogre::Vector3(event.x, 0, event.y) - cameraPosition;
			Ogre::Ray ray(rayStart, direction);
			//Ogre::Vector3 pos = ray.getPoint((rayStart - Ogre::Vector3(cameraPosition.x, cameraPosition.y - 200, cameraPosition.z - 100)).length());
			//std::cout << "Intersects at: " << pos << std::endl;
			Unit* unit = checkIfRayIntersectsWithUnits(ray);
			if (unit == NULL)
			{
				std::cout << "Not colliding with unit" << std::endl;
				unitsLock.lock();
				for (auto u : *localCopyOfUnits)
				{
					std::cout << std::endl << "Unit: " << u->getPosition() << std::endl;
				}
				unitsLock.unlock();
			}
			else
			{
				std::cout << "Ray intersected with unit with ID: " << unit->getUnitID() << std::endl;
			}*/
		}
	}
}

void Client::receiveMessages()
{
	while (true)
	{
		char buffer[1024];
		int bytesReceived = Messages::receiveMessage(sock, buffer, 1024);
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
					else if (inPosition)
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
				else if (inPosition)
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
			tank->setRotation(Ogre::Degree(unit.rotation));
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
		for (auto realUnit : *localCopyOfUnits)
		{
			if (realUnit->getUnitID() == unit.id)
			{
				realUnit->setPosition(unit.position);
				realUnit->setPlayerControlledBy(unit.playerID);
				realUnit->setRotation(Ogre::Degree(unit.rotation));
			}
		}
	}
	unitsToUpdateLock.unlock();

	//if (clientMode == CLIENT_MODE_PASSIVE)
	{
		unitsLock.lock();
		for (auto unit : *localCopyOfUnits)
		{
			//std::cout << "Unit: " << unit->getUnitID() << " is at: " << unit->getPosition() << std::endl;
			//unit->update(game->getCurrentLevel());
		}
		unitsLock.unlock();
	}
	//unitsToCreateLock.unlock();

	// Ask server for terrain
	char buffer[1024];
	buffer[0] = 0x01;
	buffer[1] = 0x00;
	buffer[2] = sizeof(int) * 4;
	buffer[3] = ((int)(cameraNode->getPosition().x - 1500)) & 0xFF000000;
	buffer[4] = ((int)(cameraNode->getPosition().x - 1500)) & 0x00FF0000;
	buffer[5] = ((int)(cameraNode->getPosition().x - 1500)) & 0x0000FF00;
	buffer[6] = ((int)(cameraNode->getPosition().x - 1500)) & 0x000000FF;
	buffer[7] = ((int)(cameraNode->getPosition().x + 1500)) & 0xFF000000;
	buffer[8] = ((int)(cameraNode->getPosition().x + 1500)) & 0x00FF0000;
	buffer[9] = ((int)(cameraNode->getPosition().x + 1500)) & 0x0000FF00;
	buffer[10] = ((int)(cameraNode->getPosition().x + 1500)) & 0x000000FF;
	buffer[11] = ((int)(cameraNode->getPosition().z - 1500)) & 0xFF000000;
	buffer[12] = ((int)(cameraNode->getPosition().z - 1500)) & 0x00FF0000;
	buffer[13] = ((int)(cameraNode->getPosition().z - 1500)) & 0x0000FF00;
	buffer[14] = ((int)(cameraNode->getPosition().z - 1500)) & 0x000000FF;
	buffer[15] = ((int)(cameraNode->getPosition().z + 1500)) & 0xFF000000;
	buffer[16] = ((int)(cameraNode->getPosition().z + 1500)) & 0x00FF0000;
	buffer[17] = ((int)(cameraNode->getPosition().z + 1500)) & 0x0000FF00;
	buffer[18] = ((int)(cameraNode->getPosition().z + 1500)) & 0x000000FF;
	//int bytesSent = Messages::sendMessage(sock, buffer, 19);
	//if (bytesSent != 19)
	{
	//	printf("Failed to send message!");
	}
	//printf("Sent message to server for updated info on world in cameras view\n");

}