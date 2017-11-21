#include <iostream>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

int main()
{
	// Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int port = 80000;

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cerr << "Can't initalize Winsock! Quitting!" << std::endl;
		return 0;
	}


	// Create a socket
	SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listeningSocket == INVALID_SOCKET)
	{
		std::cerr << "Can't create a server socket! Quitting!" << std::endl;
		return 0;
	}


	// Bind the socket to an ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port); //host to network short
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton ....

	bind(listeningSocket, (sockaddr*)&hint, sizeof(hint));


	// Tell Winsock the socket is for listening
	listen(listeningSocket, SOMAXCONN);


	// Wait for connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listeningSocket, (sockaddr*)&client, &clientSize);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cerr << "Can't create a client socket! Quitting!" << std::endl;
		return 0;
	}

	char host[NI_MAXHOST];		// Client's remote address
	char service[NI_MAXSERV];	//Serive (i.e. port) the client connect on

	ZeroMemory(host, NI_MAXHOST);		//same as memset(host, 0, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << host << " connected on port " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
	}


	// Close listening socket
	closesocket(listeningSocket);

	// While loop: accept and echo message back to client
	char buf[4096];

	ZeroMemory(buf, 4096);

	// Wait for client to send data
	char bytesReceived = recv(clientSocket, buf, 4096, 0);
	if (bytesReceived == SOCKET_ERROR)
	{
		std::cerr << "Error in recv(). Quitting!" << std::endl;
		std::cerr << "Error #" << GetLastError() << std::endl;
	}

	if (bytesReceived == 0)
	{
		std::cout << "Client disconnected." << std::endl;
	}

	char temp;

	// Echo message back to client
	for (int i = 0; i < bytesReceived; i++)
	{
		temp = buf[i];
		std::cout << sizeof(temp) << std::endl;
		send(clientSocket, &temp, sizeof(temp), 0);
	}


	// Close socket
	closesocket(clientSocket);

	//Shutdown winsock
	WSACleanup();

	return 0;
}