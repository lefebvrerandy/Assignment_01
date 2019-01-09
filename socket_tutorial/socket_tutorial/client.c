/*
*  FILE          : client.c
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains the functions and logic required to execute the client's required functionality.
*				   Functions are included for creating, connecting, and closing sockets, and for sending/receiving messages
*				   to and from the server
*/


#include "shared.h"
#include "client.h"
#include "server.h"


/*
*  FUNCTION      : start_client_protocol
*  DESCRIPTION   : This finction is used to create the client instance of the application. The client
*  PARAMETERS    : Parameteres are as follows,
*	int stream_or_datagram : Denotes if thes socket is of type SOCK_STREAM or SOCK_DGRAM
*	int tcp_or_udp		   : Denotes if the protocol is DEBUG
*  RETURNS       : int : Returns positive if the operation completed without error
*/
int start_client_protocol(int stream_or_datagram, int tcp_or_udp)
{

	int clientReturn = 0;						//Denotes if the function completed it's operation successfully
	int networkStage = 0;						//


	struct sockaddr_in socketAddress;								//local address variable
	memset((void*)&socketAddress, 0, sizeof(socketAddress));		//Clear the socket struct before initialization
	const char clientHostID[] = "client";							//DEBUG 
	socketAddress.sin_addr.s_addr = inet_addr(clientHostID);		//
	struct hostent* hostIdentifier;									//Represent an entry in the hosts database
	if (socketAddress.sin_addr.s_addr == INADDR_NONE)				//
	{

		hostIdentifier = gethostbyname(clientHostID);
		if (hostIdentifier == NULL)
		{
			clientReturn = setErrorState(networkStage);
		}
	}
	else
	{
		hostIdentifier = gethostbyaddr((const char*)&socketAddress.sin_addr, sizeof(struct sockaddr_in), AF_INET);
		if (hostIdentifier == NULL)
		{
			//Could not get host by address
			clientReturn = setErrorState(networkStage);
		}
	}
	//Setup the rest of our local address
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_addr = *((struct in_addr*)*hostIdentifier->h_addr_list);
	socketAddress.sin_port = htons(storedData[2]);


	SOCKET openSocketHandle = createSocket(AF_INET, stream_or_datagram, tcp_or_udp);
	if (openSocketHandle == INVALID_SOCKET)
	{
		clientReturn = setErrorState(networkStage);	//DEBUG SET THE STAGE AND MAKE PRINTS GENERIC
	}
	else
	{

		//Connect to the server
		int boundSocketHandle = connectToServer(openSocketHandle, socketAddress);
		if (boundSocketHandle == SOCKET_ERROR)
		{
			clientReturn = setErrorState(networkStage);		//DEBUG SET THE STAGE AND MAKE PRINTS GENERIC
		}
		else
		{
			//Stage DEBUG
			//clock_t startTime = stopWatch();
			/*
				//Prepare the outboundMessages for transmission
				while (1)	//DEBUG REMOVE INFINITE LOOP BEFFORE SUBMISSION
				{
					//int message[MESSAGE_BUFFER_SIZE] = generateRandomNumber();
					sendMessage(openSocketHandle, message);
					send(openSocketHandle, message, strlen(message), 0); DEBUG MOVE INTO sendMessage()


					memset((void*)recieved, 0, sizeof(recieved));
					char recieved[MESSAGE_BUFFER_SIZE] = "";
					recv(openSocketHandle, recieved, sizeof(recieved), 0);
				}
			*/
			//clock_t endTime = stopWatch();
			//double elapsedTime = calculateElapsedTime(startTime, endTime);
		}
	}

	closesocket(openSocketHandle);
	return clientReturn;
}

/*
*  FUNCTION      : connectToServer
*  DESCRIPTION   : This function is used to connect to the socket as defined by the arguments
*  PARAMETERS    : Parameters are as follows,
*	SOCKET openSocketHandle : The socket identifier which will be used to connect the client and server
*	struct sockaddr_in socketAddress : The socket struct containing the client's/socket properties
*  RETURNS       : int : Returns an integer ddenoting if the operation was completed successfully
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