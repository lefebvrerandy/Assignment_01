/*
*  FILE          : shared.c
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains a series of functions required by both client and server applications. 
*/


#include "shared.h"
#include "client.h"
#include "server.h"


/*
*  FUNCTION      : createSocket
*  DESCRIPTION   : This function is used to create and initialize a socket with the appropriate properties as set by the parameters
*  PARAMETERS    : parameters are as follows
*	int addressFamily : AF_INET or PF_INET
*	int socketType	: SOCK_STREAM or SOCK_DGRAM
*	int protocolType: DEBUG
*  RETURNS       : SOCKET : Returns an initialized socket
*/
SOCKET createSocket(int addressFamily, int socketType, int protocolType)
{
	SOCKET newSocket = socket(addressFamily, socketType, protocolType);
	return newSocket;

}//Done


/*
*  FUNCTION      : sendMessage
*  DESCRIPTION   : DEBUG
*  PARAMETERS    : DEBUG
*  RETURNS       : int : Returns an integer indicating if the process was a success or failure
*/
int sendMessage(SOCKET connectedSocket, int message[])
{
	int sendStatus = 0;
	//int messageBuffer[MESSAGE_BUFFER_SIZE] = "";
	//memset((void*)messageBuffer, 0, sizeof(messageBuffer));
	//sendStatus = send(acceptedSocketConnection, messageBuffer, strlen(messageBuffer), 0);


	return sendStatus;
}


/*
*  FUNCTION      : receiveMessage
*  DESCRIPTION   : DEBUG
*  PARAMETERS    : DEBUG
*  RETURNS       : int : Returns an integer indicating if the process was a success or failure
*/
int receiveMessage()
{
	int receiveStatus = 0;
	return receiveStatus;
	//DEBUG INCOMPLETE
}

/*
*  FUNCTION      : setErrorState
*  DESCRIPTION   : This function is used to print an error to the console window, and set the return to a negative indicating an error has occurred during execution
*  PARAMETERS    : This function takes no arguments
*  RETURNS       : int : Returns -1 as defined by the constant ERROR
*/
int setErrorState(int connectionStage)
{
	switch (connectionStage)
	{
		case 0:
			perror("[ERROR]: Could not create socket");
			break;

		case 1:
			perror("[ERROR]: Could not bind to the socket");
			break;

		case 2:
			perror("[ERROR]: Could not listen to the socket");
			break;

		case 3:
			perror("[ERROR]: Could not accept new connection");
			break;

		default:
			perror("[ERROR]: Unidentified error has occurred");
			break;

	}

	return ERROR;
}//Done