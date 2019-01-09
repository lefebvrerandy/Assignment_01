/*
*  FILE          : client.c
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains DEBUG
*
*  NOTE: DEBUG ADD THE REFERENCE TO NORBERTS PREVIOUS WORK, AND THE ONLINE POST HE GOT IT FROM
*/


#include "shared.h"
#include "client.h"
#include "server.h"


/*
*  FUNCTION      : start_client_protocol
*  DESCRIPTION   : DEBUG
*  PARAMETERS    : DEBUG
*  RETURNS       : DEBUG
*/
int start_client_protocol(int tcp_or_udp)
{
	struct hostent* hostIdentifier;			//Represent an entry in the hosts database
	const char local_host[] = "localhost";

	//clock_t startTime = stopWatch();
	/*
		Do all the work here
	*/
	//clock_t endTime = stopWatch();
	//double elapsedTime = calculateElapsedTime(startTime, endTime);


	struct sockaddr_in socketAddress;								//local address variable
	memset((void*)&socketAddress, 0, sizeof(socketAddress));		//Clear the socket struct before initializing it
	socketAddress.sin_addr.s_addr = inet_addr(local_host);			//
	if (INADDR_NONE == socketAddress.sin_addr.s_addr)
	{
		hostIdentifier = gethostbyname(local_host);
		if (NULL == hostIdentifier)
		{
			perror("Could not get host by name");
			return ERROR;
		}
	}
	else
	{
		hostIdentifier = gethostbyaddr((const char*)&socketAddress.sin_addr, sizeof(struct sockaddr_in), AF_INET);
		if (NULL == hostIdentifier) {
			perror("Could not get host by address");
			return ERROR;
		}
	}


	SOCKET openSocketHandle = createSocket(AF_INET, SOCK_STREAM, tcp_or_udp);
	if (openSocketHandle == INVALID_SOCKET)
	{
		perror("[ERROR]: Could not open the client socket");
		return ERROR;
	}


	// setup the rest of our local address
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_addr = *((struct in_addr*)*hostIdentifier->h_addr_list);
	socketAddress.sin_port = htons(storedData[2]);


	// connect to the server
	int boundSocketHandle = connectToServer(openSocketHandle, );
	if (boundSocketHandle == SOCKET_ERROR)
	{
		perror("[ERROR]: Cannot connect to server");
		return ERROR;
	}


	//Recieve the servers reply
	char message[] = "outBoundMessage\0";
	char recieved[MESSAGE_BUFFER_SIZE] = "";
	memset((void*)recieved, 0, sizeof(recieved));
	recv(openSocketHandle, recieved, sizeof(recieved), 0);
	printf("Server sent: %s\n", recieved);
	send(openSocketHandle, message, strlen(message), 0);


	closesocket(openSocketHandle);
	return 0;
}

/*
*  FUNCTION      : connectToServer
*  DESCRIPTION   : DEBUG
*  PARAMETERS    : DEBUG
*  RETURNS       : DEBUG
*/
int connectToServer(SOCKET openSocketHandle, struct sockaddr_in socketAddress)
{
	int newBoundSocket = connect(openSocketHandle, (struct sockaddr*)&socketAddress, sizeof(struct sockaddr));
	return newBoundSocket;

}//Done


/*
*  FUNCTION      : stopWatch
*  DESCRIPTION   : This function is used to get the number of clock ticks since the process started. It returns the tick count
*				   and is used in conjunction with calculateElapsedTime() to calulcate the total time requried for an operation to complete
*  PARAMETERS    : void: The function takes no arguments
*  RETURNS       : clock_t : Returns the number of clock ticks since the process was started
*
*	NOTE: This function  was initially found online, however, the original soruce code has since been modified to suit the projects needs.
*		   As a result, partial credit belongs to the original authors on the website. For more information, please see the reference,
*		   GeeksForGeeks.(ND). How to measure time taken by a function in C?. Retrieved on January 8, 2019,
*			from https://www.geeksforgeeks.org/how-to-measure-time-taken-by-a-program-in-c/
*/
clock_t stopWatch(clock_t startTime, clock_t endTime)
{
	clock_t clockTime = clock();
	return clockTime;

}//Done


/*
*  FUNCTION      : calculateElapsedTime
*  DESCRIPTION   : This function is used to calculate the elapsed time for message transimssion between the networked clients and server
*  PARAMETERS    : clock_t startTime : Start time for when the transmission began
*				   clock_t endTime	 : End time for when the transmission had finished
*  RETURNS       : double : Returns the elepased time between the two clock_t values
*/
double calculateElapsedTime(clock_t startTime, clock_t endTime)
{
	double elapsedTime = ((double)(endTime - startTime)) / CLOCKS_PER_SEC;
	return elapsedTime;

}//Done