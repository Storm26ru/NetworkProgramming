#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <iostream>
#include<FormatLastError.h>


using namespace std;

#pragma comment (lib, "Ws2_32.lib")
#pragma comment(lib, "FormatLastError.lib")


#define DEFAULT_PORT "27015"

void main()
{
	setlocale(LC_ALL, "");
	
	WSADATA wsaData;
	INT iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult)
	{
		cout << "WSAStartup() failed with code " << iResult << endl;
		return;
	}
	
	addrinfo* result = NULL;
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult)
	{
		cout << "getaddrinfo() failed with code" << iResult << endl;
		WSACleanup();
		return;
	}
	SOCKET connect_socket = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
	if (connect_socket == INVALID_SOCKET)
	{
		PrintLastError(WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

}