#ifndef _MESSAGES_H
#define _MESSAGES_H

#include <stdio.h>
#include <winsock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib,"ws2_32.lib")

namespace Messages
{
	int initMessages();

	SOCKET createSocket();

	int connectToServer(SOCKET sock, struct sockaddr_in a);

	int sendMessage(SOCKET socket, char* message, const int length);

	int receiveMessage(SOCKET sock, char* buffer, const int bufferSize);

	int receiveMessage(SOCKET sock, char* buffer, const int bufferSize, int flags);

	void closeSocket(SOCKET sock);

	void cleanUpMessages();

	void bindSocket(SOCKET sock, struct sockaddr_in a);

	void listenForConnections(SOCKET sock);

	SOCKET acceptConnection(SOCKET sock);
}

#endif