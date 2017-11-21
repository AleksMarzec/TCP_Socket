#pragma once
#include <WS2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")


class TcpClient
{
private:
	WSAData WinSockData;
	WORD version;

	int port;

	std::string IPadress;
	SOCKET ClientSocket;
	sockaddr_in SocketAddresses;

	void initializeWinSock();
	void createSocket();
	void connectToServer();
	void closeClean();
public:
	TcpClient();
	TcpClient(const int& port);
	~TcpClient();

	void run();
};

