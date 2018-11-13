#include "Client.h"
#include <stdio.h>
#include <stdlib.h>
#include "Game.h"

Client::Client(std::string ip, int port, Game* game, Ogre::SceneManager* sceneManager)
{
	localCopyOfUnits = new std::vector<Unit*>();
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
	printf("Sent message to server for basic info\n");

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

void Client::handleClick(const OgreBites::MouseButtonEvent& event)
{
	if (event.type == MOUSEBUTTONDOWN)
	{
		if (event.button == BUTTON_LEFT)
		{
			printf("Left click\n");
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
			printf("Receiving unit add message\n");
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

			int id = -1;
			Ogre::Vector3 position(0,0,0);
			Ogre::Real rotation = 0;
			Ogre::Vector3 scale(0,0,0);
			int playerID = 0;
			int type = UNIT_TANK;

			char* token = strtok(buffer, ">");
			while (token != NULL)
			{
				std::string tmp = token;
				if (tmp == "<ID")
				{
					inID = true;
				}
				else if (tmp == "<Position")
				{
					inPosition = true;
				}
				else if (tmp == "<X" && inPosition)
				{
					inPositionX = true;
				}
				else if (tmp == "<Y" && inPosition)
				{
					inPositionY = true;
				}
				else if (tmp == "<Z" && inPosition)
				{
					inPositionZ = true;
				}
				else if (tmp == "<Rotation")
				{
					inRotation = true;
				}
				else if (tmp == "<Scale")
				{
					inScale = true;
				}
				else if (tmp == "<X" && inScale)
				{
					inScaleX = true;
				}
				else if (tmp == "<Y" && inScale)
				{
					inScaleY = true;
				}
				else if (tmp == "<Z" && inScale)
				{
					inScaleZ = true;
				}
				else if (tmp == "<PlayerID")
				{
					inPlayerID = true;
				}
				else if (tmp == "<Type")
				{
					inType = true;
				}

				if (inID)
				{
					id = std::atoi(tmp.substr(0,tmp.find("</ID")).c_str());
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
				token = strtok(NULL, ">");
			}
			if (type == UNIT_TANK)
			{
				Tank* tank = new Tank(position, sceneManager, playerID, id);
				tank->setRotation(Ogre::Degree(rotation));
				unitsLock.lock();
				localCopyOfUnits->push_back(tank);
				unitsLock.unlock();
				printf("Received and processed unit add message\n");
			}
		}
	}
}

void Client::update(Ogre::SceneNode* cameraNode)
{
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