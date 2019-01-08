/*
*  FILE          : client.h
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains DEBUG
*
*  NOTE: DEBUG ADD THE REFERENCE TO NORBERTS PREVIOUS WORK, AND THE ONLINE POST HE GOT IT FROM
*/



//Prototypes
int validateAddress(char string[]);
int validatePort(char string[]);
int validateBlockSize(char string[]);
int validateNumOfBlocks(char string[]);
int start_server_protocol(int tcp_or_udp);
int start_client_protocol(int tcp_or_udp);
SOCKET createSocket(int protocolDomain, int socketType, int protocolType);
int connectToServer(SOCKET openSocketHandle, struct sockaddr_in socketAddress);
clock_t stopWatch(clock_t startTime, clock_t endTime);
double calculateElapsedTime(clock_t startTime, clock_t endTime);


// Global struct for all client connection info
char storedData[SWITCH_OPTIONS][MAX_ARGUMENT_LENGTH];
// storedData Breakdown
//	[0][] = TCP || UDP
//	[1][] = IP Address
//	[2][] = Port
//	[3][] = Size of buffer to send
//	[4][] = Number of blocks to send



