/*
*  FILE          : shared.h
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains the definitions, prototypes, and global constants used throughout the entirety of the application. 
*/



//Standard C headers
#include <string.h>
#include <stdio.h>
#include <time.h>



//OS Dependent Headers
#if defined _WIN32
#include <winsock.h>		//Windows socket operations
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



//Defined constants
#pragma once
#define MESSAGE_BUFFER_SIZE 256
#define SWITCH_OPTIONS 5
#define MAX_ARGUMENT_LENGTH 15
#define ERROR -1



//Prototypes
int start_server_protocol(int tcp_or_udp);
int start_client_protocol(int tcp_or_udp);
int sendMessage();
int receiveMessage();
SOCKET createSocket(int protocolDomain, int socketType, int protocolType);



//Global struct for all client connection info
char storedData[SWITCH_OPTIONS][MAX_ARGUMENT_LENGTH];
// storedData Breakdown
//	[0][] = TCP || UDP
//	[1][] = IP Address
//	[2][] = Port
//	[3][] = Size of buffer to send
//	[4][] = Number of blocks to send


