/*
*  FILE          : shared.h
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains DEBUG
*
*  NOTE: DEBUG ADD THE REFERENCE TO NORBERTS PREVIOUS WORK, AND THE ONLINE POST HE GOT IT FROM
*/


//Standard C headers
#include <string.h>
#include <stdio.h>
#include <time.h>


//OS Dependent Headers
#if defined _WIN32
#include <winsock.h>		// WinSock subsystem
#include <windows.h>		//
#elif defined __linux__	
#include <unistd.h>			//
#include <netdb.h>			//
#include <sys/types.h>		//
#include <sys/socket.h>		//
#include <arpa/inet.h>		//
#include <pthread.h>		//
#endif


//Redefine some types and constants based on OS
#if defined _WIN32
typedef int socklen_t;  // Unix socket length
#elif defined __linux__
typedef int SOCKET;
#define INVALID_SOCKET -1			// WinSock invalid socket
#define SOCKET_ERROR   -1			// basic WinSock error
#define closesocket(s) close(s);	// Unix uses file descriptors, WinSock doesn't...
#endif


//Defined constants
#pragma once
#define MESSAGE_BUFFER_SIZE 256
#define SWITCH_OPTIONS 5
#define MAX_ARGUMENT_LENGTH 15
#define ERROR -1