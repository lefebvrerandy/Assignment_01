/*
*  FILE          : shared.h
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains the definitions, prototypes, and global constants used throughout the entirety of the application.
*/
#pragma once


//Disable warnings of things that may be considered unsafe if not watched properly 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable: 4996)


//Standard C headers
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>


//OS Dependent Headers
#if defined _WIN32
#include <winsock2.h>		//Windows socket operations
#include <WS2tcpip.h>
#include <windows.h>		//Windows API for 32/64 bit application
#elif defined __linux__	
#include <unistd.h>			//UNIX POSIX operating system API
#include <netdb.h>			//UNIX network database operations
#include <sys/types.h>		//UNIX clock & threading operations
#include <sys/socket.h>		//UNIX socket operations
#include <arpa/inet.h>		//UNIX internet operations
#include <pthread.h>		//UNIX threading operations
#endif


//Redefine some types and constants based on OS
#if defined _WIN32
	//DEBUG need to fill in
#elif defined __linux__
typedef int SOCKET;
	#define INVALID_SOCKET -1			// WinSock invalid socket DEBUG never used
	#define SOCKET_ERROR   -1			// basic WinSock error	  DEBUG never used
	#define closesocket(s) close(s);	// Unix uses file descriptors, WinSock doesn't... DEBUG change comment
#endif


//Defined application constants
#define MESSAGE_BUFFER_SIZE_10000	10000
#define MESSAGE_BUFFER_SIZE_5000	5000
#define MESSAGE_BUFFER_SIZE_2000	2000
#define MESSAGE_BUFFER_SIZE_1000	1000
#define MAX_ARGUMENT_LENGTH			15
#define SWITCH_OPTIONS				5
#define ERROR						-1


//Network error states
#define SOCKET_CREATION_ERROR	-1
#define SOCKET_BIND_ERROR		-2
#define SOCKET_LISTEN_ERROR		-3
#define SOCKET_CONNECTION_ERROR -4
#define SOCKET_SEND_ERROR		-5
#define SOCKET_RECEIVE_ERROR	-6
#define SOCKET_HOST_ERROR		-7


//Location ID's for storedDatas command line arguments
#define CLA_SOCKET_TYPE			0
#define CLA_IP_ADDRESS			1
#define CLA_PORT_NUMBER			2
#define CLA_BUFFER_SIZE			3
#define CLA_NUMBER_OF_BLOCKS	4


//Global struct for all client connection info
char storedData[SWITCH_OPTIONS][MAX_ARGUMENT_LENGTH];
/* storedData Breakdown: 
	[0][] = TCP || UDP
	[1][] = IP Address
	[2][] = Port
	[3][] = Size of buffer to send
	[4][] = Number of blocks to send 
*/


//Prototypes
SOCKET createSocket(int protocolDomain, int socketType, int protocolType);
int sendMessage(SOCKET connectedSocket, char messageBuffer[]);
int receiveMessage(SOCKET connectedSocket, char messageBuffer[]);
int setErrorState(int errorState);


