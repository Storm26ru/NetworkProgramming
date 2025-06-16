﻿#define _CRT_SECURE_NO_WARNINGS﻿
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN


#include<Windows.h>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<iphlpapi.h>
#include<stdio.h>
#include<iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT	"27015"

LPSTR FormatLastError(DWORD dwMessageID);
VOID PrintLastError(DWORD dwMessageID);

void main()
{
	setlocale(LC_ALL, "Russian");
	cout << "WinSock Client" << endl;
	//1) Инициализация WinSock:
	WSADATA wsaData;
	INT iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult)
	{
		cout << "WSAStartup() failed with code " << iResult << endl;
		return;
	}

	//2) Создаем ClientSocket:
	addrinfo* result = NULL;
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;	//INET - TCP/IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//3) Определяем IP-адрес Сервера:
	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult)
	{
		cout << "getaddressinfo() failed with code " << iResult << endl;
		WSACleanup();
		return;
	}
	//cout << "hints:" << endl;
	//cout << "ai_addr:" << hints.ai_addr->sa_data << endl;

	SOCKET connect_socket = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
	if (connect_socket == INVALID_SOCKET)
	{
		cout << "Error: soket creatin failed with code: " << WSAGetLastError();
		DWORD dwMessageID = WSAGetLastError();
		
		freeaddrinfo(result);
		WSACleanup();
		return;
	}
	//4) Подключение к Серверу:
	iResult = connect(connect_socket, result->ai_addr, result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		DWORD dwMessageID = WSAGetLastError();
		LPSTR szMessage = FormatLastError(dwMessageID);
		cout << "Error" << dwMessageID << ":" << szMessage << endl;
		closesocket(connect_socket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}
	//?) Освобождаем ресурсы WinSock:
	WSACleanup();
}
LPSTR FormatLastError(DWORD dwMessageID)
{
	LPSTR szBuffer = NULL;
	FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
		dwMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_RUSSIAN_RUSSIA), (LPSTR)&szBuffer, 0, NULL

	);
	return szBuffer;
}
VOID PrintLastError(DWORD dwMessageID)
{

};