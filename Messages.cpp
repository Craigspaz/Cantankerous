#include "Messages.h"

int Messages::initMessages()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("ERROR: Failed to initialize sockets: %d\n", WSAGetLastError());
		return 1;
	}
	printf("Initialized sockets\n");
	return 0;
}

SOCKET Messages::createSocket()
{
	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		printf("ERROR: Failed to initialize socket: %d\n", WSAGetLastError());
	}
	printf("Created Socket\n");
	return sock;
}


int Messages::connectToServer(SOCKET sock, struct sockaddr_in a)
{
	if (connect(sock, (struct sockaddr*)&a, sizeof(a)) < 0)
	{
		printf("ERROR: Failed to connect to server\n");
		return -1;
	}
	printf("Connected to server\n");
	return 0;
}


int Messages::sendMessage(SOCKET socket, char* message, const int length)
{
	int bytesSend = send(socket, message, length, 0);
	if (bytesSend < 0)
	{
		printf("ERROR: Failed to send message\n");
		return 0;
	}
	//printf("Sent Data\n");
	return bytesSend;
}


int Messages::receiveMessage(SOCKET sock, char* buffer, const int bufferSize)
{
	int size = recv(sock, buffer, bufferSize, 0);
	if (size == SOCKET_ERROR)
	{
		printf("ERROR: Failed to receive a message");
		buffer = NULL;
		return 0;
	}
	return size;
}

int Messages::receiveMessage(SOCKET sock, char* buffer, const int bufferSize, int flags)
{
	int size = recv(sock, buffer, bufferSize, flags);
	if (size == SOCKET_ERROR)
	{
		printf("ERROR: Failed to receive a message");
		buffer = NULL;
		return 0;
	}
	return size;
}


void Messages::closeSocket(SOCKET sock)
{
	closesocket(sock);
}

void Messages::cleanUpMessages()
{
	WSACleanup();
}


void Messages::bindSocket(SOCKET sock, struct sockaddr_in a)
{
	if (bind(sock, (struct sockaddr*)&a, sizeof(a)) == SOCKET_ERROR)
	{
		printf("ERROR: Failed to bind port: %d\n", WSAGetLastError());
		return;
	}
	printf("Binded port successfully\n");
}


void Messages::listenForConnections(SOCKET sock)
{
	listen(sock, 3);
}


SOCKET Messages::acceptConnection(SOCKET sock)
{
	struct sockaddr_in client;
	int c = sizeof(struct sockaddr_in);
	SOCKET newSocket = accept(sock, (struct sockaddr*)&client, &c);
	if (newSocket == INVALID_SOCKET)
	{
		printf("ERROR: Failed to accept connection\n");
		return newSocket;
	}
	printf("Accepted connection\n");
	return newSocket;
}