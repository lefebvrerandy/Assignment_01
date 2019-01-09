/*
*  FILE          : client.h
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains the function prototypes used by the clients to execute their functionality in full
*/

#include "shared.h"

//Function prototypes
int connectToServer(SOCKET openSocketHandle, struct sockaddr_in socketAddress);
clock_t stopWatch(clock_t startTime, clock_t endTime);
double calculateElapsedTime(clock_t startTime, clock_t endTime);