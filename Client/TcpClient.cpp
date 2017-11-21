#include "TcpClient.h"
#include <iostream>


// Default constructor
TcpClient::TcpClient()
{
	this->port = 80000;
	this->IPadress = "127.0.0.1";
	initializeWinSock();
	createSocket();
}

// Constructor with user specified listening port
TcpClient::TcpClient(const int& port)
{
	this->port = port;
	this->IPadress = "127.0.0.1";
	initializeWinSock();
	createSocket();
}


TcpClient::~TcpClient()
{

}

void TcpClient::initializeWinSock()
{
	version = MAKEWORD(2, 2);

	// Handling errors
	if (WSAStartup(version, &WinSockData) != 0)
	{
		std::cerr << "Can't start WinSock." << std::endl;
		std::cerr << "Error #" << WSAGetLastError() << "." << std::endl;
		return;
	}
	else
	{
		std::cout << "WinSock start succes." << std::endl;
	}
}

// Creates client socket. Function required in constructor.
void TcpClient::createSocket()
{
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Handling errors and exceptions
	if (ClientSocket == INVALID_SOCKET)
	{
		std::cerr << "Can't create a client socket." << std::endl;
		std::cerr << "Error #" << WSAGetLastError() << std::endl;
		return;
	}
	else
	{
		std::cout << "Client socket successfully created." << std::endl;
	}

	SocketAddresses.sin_family = AF_INET;
	SocketAddresses.sin_port = htons(this->port);
	inet_pton(AF_INET, IPadress.c_str(), &SocketAddresses.sin_addr);
}

// Connects client socket to server
void TcpClient::connectToServer()
{
	// Handling errors and exceptions
	int errorFlag = connect(ClientSocket, (sockaddr*)&SocketAddresses, sizeof(SocketAddresses));
	if (errorFlag == SOCKET_ERROR)
	{
		std::cerr << "Can't connect to server." << std::endl;
		std::cerr << "Error #" << WSAGetLastError() << std::endl;
		closeClean();
		return;
	}
	else
	{
		std::cout << "Connected to server." << std::endl;
	}
}

// Closes client socket and cleans WS
void TcpClient::closeClean()
{
	closesocket(ClientSocket);
	WSACleanup();		// Deregisters itself from a Windows Sockets implementation and allows 
						//the implementation to free any resources allocated on behalf of the application or DLL
}

void TcpClient::run()
{
	connectToServer();

	char buffer;
	std::string userInput;

	std::cout << "\nSEND DATA" << std::endl;
	std::cout << ">> ";
	std::getline(std::cin, userInput);

	if (userInput.size() > 0)				// Checks if user has typed in something 
	{
		int sendDataError = send(ClientSocket, userInput.c_str(), userInput.size() + 1, 0);		// Sends user input data

		if (sendDataError != SOCKET_ERROR)
		{
			while (true)		// Receives server response
			{
				ZeroMemory(&buffer, 1);		// Clears a block of memory
				int receivedBytes = recv(ClientSocket, &buffer, 1, 0);

				if (receivedBytes > 0)
				{
					std::cout << "SERVER_RESPONSE> " << std::string(&buffer, 0, receivedBytes) << std::endl;
				}
				if (receivedBytes == 0)
				{
					std::cout << "Server has ended the connection." << std::endl;
					break;
				}
			}
		}
	}
	else
	{
		std::cerr << "Closing program. No input." << std::endl; // Exception
	}

	closeClean();
}
