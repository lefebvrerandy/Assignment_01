/*
*  FILE          : server.h
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains the function prototypes used by the server application to execute it's functionality in full
*/


#include "shared.h"
#pragma comment (lib, "ws2_32.lib")


//Struct containing the message properties as set by the clients CLA's
typedef struct {
	long blockSize;
	int blockCount;

}MessageProperties;


//Struct used to track the networks performance
typedef struct {
	int prevBlockID;
	int currentBlockID;
	int bytesReceived;
	int missingBytes;
	int missingBlocks;
	int disorganizedBytes;

}NetworkResults;


//Function prototypes
int start_server();
int start_server_protocol(int* tcpOrUdp);
int validateAddress(char string[]);
int validatePort(char* string);
int validateBlockSize(char* string);
int validateNumOfBlocks(char* string);
