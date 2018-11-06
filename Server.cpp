#include "Server.h"



Server::Server()
{
	messageRecievingThread = new std::thread(&Server::waitForMessages,this);
	messageRecievingThread->detach();
}


Server::~Server()
{
}

void Server::update()
{

}

void Server::waitForMessages()
{

}

void Server::recieveMessage()
{

}

void Server::sendMessage()
{

}
