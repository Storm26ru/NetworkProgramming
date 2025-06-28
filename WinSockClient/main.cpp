
#define _CRT_SECURE_NO_WARNINGS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include<Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <iostream>
#include<FormatLastError.h>
#include <sstream>


using namespace std;

#pragma comment (lib, "Ws2_32.lib")
#pragma comment(lib, "FormatLastError.lib")


#define DEFAULT_PORT "27015"
#define DEFAULT_BUFFER_LENGTN 1500
#define SORY "Connection is currently unavailable, please try again later."
BOOL bExit = TRUE;

VOID Recv(SOCKET connect_socket);
//VOID Send(SOCKET connect_socket);


void main()
{
	setlocale(LC_ALL, "");
	cout << "WinSock Client" << endl;
	cout << "Введите сообщение: ";

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
		cout << "socket() faied " << endl;
		PrintLastError(WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	iResult = connect(connect_socket, result->ai_addr, result->ai_addrlen);
	if (iResult)
	{
		cout << "connect() failed " << endl;
		PrintLastError(WSAGetLastError());
		closesocket(connect_socket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Recv, (LPVOID)connect_socket, 0, NULL);
	//Send(connect_socket);
	CHAR sendbuffer[DEFAULT_BUFFER_LENGTN] = "Hello Server, I am client";
	do
	{
		iResult = send(connect_socket, sendbuffer, strlen(sendbuffer), 0);
		if (iResult == SOCKET_ERROR)
		{
			cout <<"send() failed " << endl;
			PrintLastError(WSAGetLastError());
			closesocket(connect_socket);
			freeaddrinfo(result);
			WSACleanup();
			return;
		}
		ZeroMemory(sendbuffer, DEFAULT_BUFFER_LENGTN);
		SetConsoleCP(1251);
		cin.getline(sendbuffer, DEFAULT_BUFFER_LENGTN);
		SetConsoleCP(866);
	} while (iResult > 0 && strcmp(sendbuffer, "exit"));
	bExit = FALSE;
	CloseHandle(hThread);
	iResult = shutdown(connect_socket, SD_SEND);
	if (iResult == SOCKET_ERROR) PrintLastError(WSAGetLastError());
	closesocket(connect_socket);
	freeaddrinfo(result);
	WSACleanup();
	system("PAUSE");



}
VOID Recv(SOCKET connect_socket)
{
	CHAR recvbuffer[DEFAULT_BUFFER_LENGTN]{};
	INT iResult = 0;
	do
	{
		iResult = recv(connect_socket, recvbuffer, DEFAULT_BUFFER_LENGTN, 0);
		if (iResult > 0)
		{
			CHAR IPaddress[DEFAULT_BUFFER_LENGTN]{};
			stringstream sStream;  sStream << recvbuffer; sStream >> IPaddress;
			ZeroMemory(recvbuffer, DEFAULT_BUFFER_LENGTN);
			sStream.getline(recvbuffer, DEFAULT_BUFFER_LENGTN);
			sStream.clear();
			cout << "-------------------------------------------------------" << endl;
			cout << "From: " << IPaddress << ", Receved bytes: " << iResult << ", Message: " << recvbuffer << endl;
			cout << "Введите сообщение: ";
		}
		else if (iResult == 0)
		{
			cout << "Connection closing" << endl; break;
		}
		else PrintLastError(WSAGetLastError()); 
		if (strcmp(recvbuffer, "Connection is currently unavailable, please try again later.") == 0) break;
		ZeroMemory(recvbuffer, DEFAULT_BUFFER_LENGTN);
	} while (bExit);
}
//VOID Send(SOCKET connect_socket)
//{
//	INT iResult = 0;
//	CHAR sendbuffer[DEFAULT_BUFFER_LENGTN] = "Hello Server, I am client";
//	do
//	{
//		//ZeroMemory(recvbuffer, DEFAULT_BUFFER_LENGTN);
//		//iResult = recv(connect_socket, recvbuffer, DEFAULT_BUFFER_LENGTN, 0);
//		iResult = send(connect_socket, sendbuffer, strlen(sendbuffer), 0);
//		//if (iResult == SOCKET_ERROR)
//		//{
//		//	/*cout << "Receved bytes: " << iResult << " Message: " << recvbuffer << endl;
//		//	cout << "Введите сообщение: ";
//		//	cin.getline(sendbuffer, DEFAULT_BUFFER_LENGTN);
//		//	if (send(connect_socket, sendbuffer, strlen(sendbuffer), 0) == SOCKET_ERROR) PrintLastError(WSAGetLastError());*/
//		//	PrintLastError(WSAGetLastError());
//		//	closesocket(connect_socket);
//		//	freeaddrinfo(result);
//		//	WSACleanup();
//		//	return;
//		//}
//		ZeroMemory(sendbuffer, DEFAULT_BUFFER_LENGTN);
//		//cout << "Введите сообщение: ";
//		SetConsoleCP(1251);
//		cin.getline(sendbuffer, DEFAULT_BUFFER_LENGTN);
//		SetConsoleCP(866);
//		//else if (iResult == 0)cout << "Connection closing" << endl;
//		//else PrintLastError(WSAGetLastError());
//	} while (iResult > 0 && strcmp(sendbuffer,"exit"));
//}