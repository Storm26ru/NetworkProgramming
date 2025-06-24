#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <iostream>
#include<FormatLastError.h>


using namespace std;

#pragma comment (lib, "Ws2_32.lib")
#pragma comment(lib, "FormatLastError.lib")


#define DEFAULT_PORT "27015"
#define DEFAULT_BUFFER_LENGTN 1500
#define SORY "Connection is currently unavailable, please try again later."

void main()
{
	setlocale(LC_ALL, "");
	cout << "WinSock Client" << endl;

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

	iResult = connect(connect_socket, result->ai_addr, result->ai_addrlen);
	if (iResult)
	{
		PrintLastError(WSAGetLastError());
		closesocket(connect_socket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	CHAR sendbuffer[DEFAULT_BUFFER_LENGTN]= "Hello Server, I am client";
	CHAR recvbuffer[DEFAULT_BUFFER_LENGTN]{};
	iResult = send(connect_socket, sendbuffer, strlen(sendbuffer), 0);
	if (iResult == SOCKET_ERROR)
	{
		PrintLastError(WSAGetLastError());
		closesocket(connect_socket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}
	do
	{
		ZeroMemory(recvbuffer, DEFAULT_BUFFER_LENGTN);
		ZeroMemory(sendbuffer, DEFAULT_BUFFER_LENGTN);
		iResult = recv(connect_socket, recvbuffer, DEFAULT_BUFFER_LENGTN, 0);
		if (iResult > 0)
		{
			cout << "Receved bytes: " << iResult << " Message: " << recvbuffer << endl;
			cout << "Введите сообщение: ";
			cin.getline(sendbuffer, DEFAULT_BUFFER_LENGTN);
			if (send(connect_socket, sendbuffer, strlen(sendbuffer), 0) == SOCKET_ERROR) PrintLastError(WSAGetLastError());
		}
		else if (iResult == 0)cout << "Connection closing" << endl;
		else PrintLastError(WSAGetLastError());
	} while (iResult > 0);

	iResult = shutdown(connect_socket, SD_SEND);
	if (iResult == SOCKET_ERROR) PrintLastError(WSAGetLastError());
	closesocket(connect_socket);
	freeaddrinfo(result);
	WSACleanup();
	system("PAUSE");



}