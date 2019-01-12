/*
*  FILE          : server.h
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains the function prototypes used by the server application to execute it's functionality in full
*/


#include "shared.h"
#pragma comment (lib, "ws2_32.lib")


//Function prototypes
int start_server();
int start_server_protocol(int* tcpOrUdp);
int validateAddress(char string[]);
int validatePort(char* string);
int validateBlockSize(char* string);
int validateNumOfBlocks(char* string);
