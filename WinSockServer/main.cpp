#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <iostream>
#include<FormatLastError.h>

using namespace std;

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"FormatLastError.lib")

#define DEFAULT_PORT			"27015"
#define DEFAULT_BUFFER_LENGTH	1500
#define MAX_THREADS 3
HANDLE hThreadArray[MAX_THREADS];
SOCKET sClientArray[MAX_THREADS];
INT iCount_client = 0;

DWORD WINAPI ThreadProc(SOCKET client_socket);

void main()
{
	setlocale(LC_ALL, "");

	WSADATA wsaData;
	INT iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		PrintLastError(WSAGetLastError());
		return;
	}

	addrinfo* result = NULL;
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		cout << "getaddrinfo() failed with ";
		PrintLastError(WSAGetLastError());
		WSACleanup();
		return;
	}

	SOCKET listen_socket = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
	if (listen_socket == SOCKET_ERROR)
	{
		cout << "socket() failed with ";
		PrintLastError(WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	iResult = bind(listen_socket, result->ai_addr, result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		cout << " bind() failed with ";
		PrintLastError(WSAGetLastError());
		closesocket(listen_socket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	iResult = listen(listen_socket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		cout << "listen() failed with ";
		PrintLastError(WSAGetLastError());
		closesocket(listen_socket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	cout << "Waiting for clients..." << endl;
	while (true)
	{
		SOCKET client_socket = accept(listen_socket, NULL, NULL);
		/*if (client_socket == INVALID_SOCKET)
		{
			cout << "accept() failed with ";
			PrintLastError(WSAGetLastError());
			closesocket(listen_socket);
			freeaddrinfo(result);
			WSACleanup();
			return;
		}*/
		if (iCount_client < MAX_THREADS)
		{
			sClientArray[iCount_client] = client_socket;
			hThreadArray[iCount_client] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, (LPVOID)sClientArray[iCount_client], 0, NULL);
			iCount_client++;
		}
		else
		{
			CONST CHAR sendbuffer[] = "Connection is currently unavailable, please try again later.";
			if (send(client_socket, sendbuffer, strlen(sendbuffer), 0) == SOCKET_ERROR)
			{
				cout << "send() failed with ";
				PrintLastError(WSAGetLastError());
			}
			closesocket(client_socket);
		}
	}

	WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);
	closesocket(listen_socket);
	freeaddrinfo(result);
	WSACleanup();
}

DWORD WINAPI ThreadProc(SOCKET client_socket)
{
	int iResult = 0;
	CHAR recvbuffer[DEFAULT_BUFFER_LENGTH]{};
	CHAR IPaddress[DEFAULT_BUFFER_LENGTH]{};
	sockaddr_in address;
	int len = sizeof(address);
	ZeroMemory(&address, len);
	getpeername(client_socket, (sockaddr*)&address, &len);
	sprintf(IPaddress, ", Socket: %s:%i", inet_ntoa(address.sin_addr), ((address.sin_port & 0xFF) << 8) + (address.sin_port >> 8));
	//cout << "IP-address: " << inet_ntoa(address.sin_addr) << " Port: " << ((address.sin_port & 0xFF) << 8) + (address.sin_port >> 8) << endl;
	do
	{
		ZeroMemory(recvbuffer, sizeof(recvbuffer));
		iResult = recv(client_socket, recvbuffer, DEFAULT_BUFFER_LENGTH, 0);
		if (iResult > 0)
		{
			//cout <<IPaddress<<", Received Bytes: " << iResult << ", Message: " << recvbuffer<< endl;
			cout <<"Received Bytes: " << iResult << ", Message: " << recvbuffer<<IPaddress<< endl;
			for (int i = 0; i < iCount_client; i++)
			{
				if (sClientArray[i]!=client_socket)
				{
					if (send(sClientArray[i], recvbuffer, strlen(recvbuffer), 0) == SOCKET_ERROR)
					{
						cout << "send() failed with ";
						PrintLastError(WSAGetLastError());
						closesocket(client_socket);
						break;
					}
				}
			}
		}
		else if (iResult == 0)
		{
			cout << "Connection closing..." << endl;
			closesocket(client_socket);
		}
		else
		{
			cout << "recv() failed with ";
			PrintLastError(WSAGetLastError());
			closesocket(client_socket);
		}
	} while (iResult > 0);

	return 1;
}