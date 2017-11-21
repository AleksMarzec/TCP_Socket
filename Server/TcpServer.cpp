#include "TcpServer.h"
#include <iostream>


TcpServer::TcpServer()
{
	this->port = 80000;
	initializeWinSock();
}

TcpServer::TcpServer(const int& port)
{
	this->port = port;
	initializeWinSock();
}


TcpServer::~TcpServer()
{
}


void TcpServer::initializeWinSock()
{
	version = MAKEWORD(2, 2);

	// Handling errors
	if (WSAStartup(version, &WinSockData) != 0)
	{
		std::cerr << "\nCan't start WinSock." << std::endl;
		std::cerr << "Error #" << WSAGetLastError() << "." << std::endl;
		WSACleanup();
		return;
	}
	else
	{
		std::cout << "WinSock start succes." << std::endl;
	}
}

void TcpServer::createListeningSocket()
{
	ListeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Handling errors
	if (ListeningSocket == INVALID_SOCKET)
	{
		std::cerr << "\nCan't create listening (server) socket." << std::endl;
		std::cerr << "Error #" << WSAGetLastError() << std::endl;
		closesocket(ListeningSocket);
		WSACleanup();
		return;
	}
	else
	{
		std::cout << "Listening socket successfully created." << std::endl;
	}
}

void TcpServer::bindSocket()
{
	SocketAddressesServer.sin_family = AF_INET;
	SocketAddressesServer.sin_port = htons(port);
	SocketAddressesServer.sin_addr.S_un.S_addr = INADDR_ANY;	// alternative: inet_pton

	bind(ListeningSocket, (sockaddr*)&SocketAddressesServer, sizeof(SocketAddressesServer));
	listen(ListeningSocket, SOMAXCONN);	// Telling WinSock socket to listen XD
}

void TcpServer::createClientSocket()
{
	int clientSocketSize = sizeof(SocketAddressesClient);

	ClientSocket = accept(ListeningSocket, (sockaddr*)&SocketAddressesClient, &clientSocketSize);
	if (ClientSocket == INVALID_SOCKET)
	{
		std::cerr << "\nCan't create client socket." << std::endl;
		std::cerr << "Error #" << WSAGetLastError() << std::endl;
		closesocket(ListeningSocket);
		closesocket(ClientSocket);
		WSACleanup();
		return;
	}
	else
	{
		std::cout << "Client socket successfully created." << std::endl;
	}
}


void TcpServer::run()
{
	// Use private methods to properly configure server
	createListeningSocket();
	bindSocket();
	std::cout << "\nWaiting for client to connect." << std::endl;
	createClientSocket();

	char hostRemoteAddress[NI_MAXHOST];
	char clientPortNumber[NI_MAXSERV];

	ZeroMemory(hostRemoteAddress, NI_MAXHOST);	// Just for safety
	ZeroMemory(clientPortNumber, NI_MAXSERV);

	// Prints client's remote address and port
	if (getnameinfo((sockaddr*)&SocketAddressesClient, sizeof(SocketAddressesClient), hostRemoteAddress, NI_MAXHOST, clientPortNumber, NI_MAXSERV, 0) == 0)
	{
		std::cout << "\n" << hostRemoteAddress << "\tport: " << clientPortNumber << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &SocketAddressesClient.sin_addr, hostRemoteAddress ,NI_MAXHOST);
		std::cout << "\n" << hostRemoteAddress << "\tport: " << ntohs(SocketAddressesClient.sin_port) << std::endl;
	}

	closesocket(ListeningSocket);		// Closing listening socket

	char buffer[4096];
	ZeroMemory(&buffer, 4096);

	// Wait for client to send data

	char dataReceived = recv(ClientSocket, buffer, 4096, 0);
	if (dataReceived == SOCKET_ERROR)
	{
		std::cerr << "\nError while receiving data from client." << std::endl;
		std::cerr << "Error #" << WSAGetLastError() << std::endl;
		closesocket(ClientSocket);
		WSACleanup();
	}
	else
	{
		std::cout << "\nCLIENT> " << std::string(buffer) << std::endl;
	}

	if (dataReceived == 0)
	{
		std::cerr << "Client disconnected." << std::endl;
		closesocket(ClientSocket);
		WSACleanup();
	}

	// Send echo message
	char tempByte;

	for (int i = 0; i < dataReceived; i++)
	{
		tempByte = buffer[i];
		send(ClientSocket, &tempByte, sizeof(tempByte), 0);
	}

	// Closing listening socket, cleanup
	closesocket(ClientSocket);
	WSACleanup();
}