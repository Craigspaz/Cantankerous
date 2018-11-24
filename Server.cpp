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
	sock = Messages::createSocket();
	connection.sin_family = AF_INET;
	connection.sin_addr.s_addr = INADDR_ANY;
	connection.sin_port = htons(1234);
	Messages::bindSocket(sock,connection);
	messageRecievingThread = new std::thread(&Server::waitForMessages,this);
	messageRecievingThread->detach();

	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	Tank* tank = new Tank(Ogre::Vector3(0, 10, 0), this->sceneManager, 1);
	addUnit(tank);
}


Server::~Server()
{
}


void Server::update()
{
	for (auto unit : *units)
	{
		unit->setPosition(unit->getPosition() + Ogre::Vector3(0.05, 0, 0));
		unit->update(game->getCurrentLevel());
		//std::cout << "Unit Position: " << unit->getPosition() << std::endl;
		setUpdateAboutUnit(unit);
	}
}

void Server::waitForMessages()
{
	while (true)
	{
		Messages::listenForConnections(sock);
		SOCKET newSocket = Messages::acceptConnection(sock);
		sockets->push_back(newSocket);
		char buffer[1024];
		printf("Receiving message from client\n");
		int bytesReceived = Messages::receiveMessage(newSocket, buffer, 1024);
		if (bytesReceived == 0)
		{
			return;
		}
		if (bytesReceived == 3 && buffer[0] == 0x09 && buffer[1] == 0x00 && buffer[2] == 0x00)
		{
			char sendBuffer[1024];
			sendBuffer[0] = 0xFF;
			sendBuffer[1] = 0x00;
			sendBuffer[2] = 0x00;
			char* tmpSendBuffer = sendBuffer + 3;
			std::string filename = "<Level>" + game->getCurrentLevelFileName() + "</Level>";
			strncpy(tmpSendBuffer, filename.c_str(), 255);
			//printf("Sending message to client\n");
			int bytesSent = Messages::sendMessage(newSocket, sendBuffer, 1024);
			if (bytesSent == 0)
			{
				return;
			}
		}
	}
}


void Server::setUpdateAboutUnit(Unit* unit)
{
	//printf("Sending unit update to clients");
	char sendBuffer[1024];
	Ogre::Vector3 facingDirection = unit->getOrientation() * Ogre::Vector3::UNIT_Z;;
	sendBuffer[0] = 0x02;
	std::string message = "<ID>" + std::to_string(unit->getUnitID());
	message += "</ID><Position><X>";
	message += std::to_string(unit->getPosition().x);
	message += "</X><Y>";
	message += std::to_string(unit->getPosition().y);
	message += "</Y><Z>";
	message += std::to_string(unit->getPosition().z);
	message += "</Z></Position><Rotation>";
	message += std::to_string(Ogre::Degree(-facingDirection.angleBetween(Ogre::Vector3::UNIT_Z)).valueDegrees());
	message += "</Rotation><Scale><X>";
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

	short length = message.length();
	sendBuffer[1] = length & 0xFF00;
	sendBuffer[2] = length & 0x00FF;
	short i = 0;
	for (i = 0; i < length; i++)
	{
		sendBuffer[i + 3] = message.at(i);
	}
	for (auto s : *sockets)
	{
		Messages::sendMessage(s, sendBuffer, length + 3);
	}
	//printf("Sent unit update message");
}


void Server::sendUnitToClients(Unit* unit)
{
	//printf("Sending unit to clients");
	char sendBuffer[1024];
	Ogre::Vector3 facingDirection = unit->getOrientation() * Ogre::Vector3::UNIT_Z;;
	sendBuffer[0] = 0x01;
	std::string message = "<ID>" + std::to_string(unit->getUnitID());
	message += "</ID><Position><X>";
	message += std::to_string(unit->getPosition().x);
	message += "</X><Y>";
	message += std::to_string(unit->getPosition().y);
	message += "</Y><Z>";
	message += std::to_string(unit->getPosition().z);
	message += "</Z></Position><Rotation>";
	message += std::to_string(Ogre::Degree(-facingDirection.angleBetween(Ogre::Vector3::UNIT_Z)).valueDegrees());
	message += "</Rotation><Scale><X>";
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

	short length = message.length();
	sendBuffer[1] = length & 0xFF00;
	sendBuffer[2] = length & 0x00FF;
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
	this->units->push_back(unit);
	sendUnitToClients(unit);
}