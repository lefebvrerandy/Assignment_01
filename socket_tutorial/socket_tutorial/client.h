/*
*  FILE          : client.h
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains DEBUG
*
*  NOTE: DEBUG ADD THE REFERENCE TO NORBERTS PREVIOUS WORK, AND THE ONLINE POST HE GOT IT FROM
*/


//Function prototypes
int connectToServer(SOCKET openSocketHandle, struct sockaddr_in socketAddress);
clock_t stopWatch(clock_t startTime, clock_t endTime);
double calculateElapsedTime(clock_t startTime, clock_t endTime);