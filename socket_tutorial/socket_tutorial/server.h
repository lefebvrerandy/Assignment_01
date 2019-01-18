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
	int bytesReceivedCount;
	int blocksReceivedCount;
	int missingBytesCount;
	int missingBlockCount;
	int disorganizedBytesCount;
	int disorganizedBlocksCount;
	int receivedBlocks[MESSAGE_BUFFER_SIZE_10000];
	int missedBlocks[MESSAGE_BUFFER_SIZE_10000];

}NetworkResults;


//Function prototypes
int start_server();
int start_server_protocol(int* tcpOrUdp);
long getBlockSize(char messageCopy[]);
int convertHexToDecimal(char* messageProperties);
int getNumberOfBlocks(char messageCopy[]);
int getBlockID(char messageCopy[]);
int checkForMissedBlock(int currentBlockID, int prevBlockID);
int getBytesMissing(int blockSize, char* messageBuffer);
void packageResults(char messagBuffer[], int packagedValue);
