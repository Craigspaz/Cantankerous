#include "Server.h"
#include "Game.h"
#include "Tank.h"


Server::Server(Game* game, Ogre::SceneManager* sceneManager)
{
	this->game = game;
	this->sceneManager = sceneManager;

	//Create test tank
	units = new std::vector<Unit*>();

	Tank* tank = new Tank(Ogre::Vector3(0,10,0), sceneManager, 1);
	this->units->push_back(tank);

	sock = Messages::createSocket();
	connection.sin_family = AF_INET;
	connection.sin_addr.s_addr = INADDR_ANY;
	connection.sin_port = htons(1234);
	Messages::bindSocket(sock,connection);
	messageRecievingThread = new std::thread(&Server::waitForMessages,this);
	messageRecievingThread->detach();
}


Server::~Server()
{
}

void Server::update()
{
	for (auto unit : *units)
	{
		unit->update();
	}
}

void Server::waitForMessages()
{
	while (true)
	{
		Messages::listenForConnections(sock);
		SOCKET newSocket = Messages::acceptConnection(sock);
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
			printf("Sending message to client\n");
			int bytesSent = Messages::sendMessage(newSocket, sendBuffer, 1024);
			if (bytesSent == 0)
			{
				return;
			}
		}
	}
}

void Server::recieveMessage()
{

}

void Server::sendMessage()
{

}
