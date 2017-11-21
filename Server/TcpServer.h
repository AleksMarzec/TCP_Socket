#pragma once
#include <WS2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

class TcpServer
{
private:
	WSAData WinSockData;
	WORD version;

	int port;

	SOCKET ListeningSocket;
	SOCKET ClientSocket;
	sockaddr_in SocketAddressesServer;
	sockaddr_in SocketAddressesClient;

	void initializeWinSock();
	void createListeningSocket();
	void bindSocket();
	void createClientSocket();

public:
	TcpServer();
	TcpServer(const int& port);
	~TcpServer();

	void run();
};

