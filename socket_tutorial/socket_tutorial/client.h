/*
*  FILE          : client.h
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains the function prototypes used by the clients to execute their functionality in full
*/


#include "shared.h"


//Function prototypes
int start_client_protocol(int stream_or_datagram, int tcp_or_udp);
int connectToServer(SOCKET openSocketHandle, struct sockaddr_in socketAddress);
void fillMessageBuffer(char messageBuffer[], int bufferSize, char numOfTimes[]);
long stopWatch(void);
double calculateElapsedTime(long startTime, long endTime);
int convertCharToInt(char* stringToConvert);
int calculateSpeed(int bytes, int elapsedTimeMS);
void printResults(int size, int sent, int time, int speed, int missing, int disordered);