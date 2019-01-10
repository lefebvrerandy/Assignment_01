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
*	int protocolType: IPPROTO_TCP or IPPROTO_UDO
*  RETURNS       : SOCKET : Returns an initialized socket
*/
SOCKET createSocket(int addressFamily, int socketType, int protocolType)
{
	SOCKET newSocket = socket(addressFamily, socketType, protocolType);
	return newSocket;

}//Done


/*
*  FUNCTION      : sendMessage
*  DESCRIPTION   : This function is used to send a message to the other networked applications, 
*				   across the supplied SOCKET
*  PARAMETERS    : The parameters are as follows,
*	SOCKET connectedSocket	: Socket through which the message will be sent
*	int message[]			: Contains the entire message
*  RETURNS       : int : Returns an integer indicating if the process was a success or failure
*/
int sendMessage(SOCKET connectedSocket, char messageBuffer[])
{
	int sendStatus = send(connectedSocket, messageBuffer, strlen(messageBuffer), 0);
	return sendStatus;
}


/*
*  FUNCTION      : receiveMessage
*  DESCRIPTION   : This function is used to receive a message from the clients/servers, and save 
*				   the return into the inbound message buffer
*  PARAMETERS    : Parameters are as follows,
*	SOCKET connectedSocket : Socket through which the messages will be received
*	int messageBuffer[]	   : Array containing the received message from the other application
*  RETURNS       : int : Returns an integer indicating if the process was a success or failure
*/
int receiveMessage(SOCKET connectedSocket, char messageBuffer[])
{
	int receiveStatus = recv(connectedSocket, messageBuffer, sizeof(messageBuffer), 0);
	return receiveStatus;

}//Done


/*
*  FUNCTION      : setErrorState
*  DESCRIPTION   : This function is used to print an error to the console window, and set the return to a negative indicating an error has occurred during execution
*  PARAMETERS    : This function takes no arguments
*  RETURNS       : int : Returns -1 as defined by the constant ERROR
*/
int setErrorState(int errorState)
{
	switch (errorState)
	{
		case SOCKET_CREATION_ERROR:
			perror("[ERROR]: Could not create socket");
			break;

		case SOCKET_BIND_ERROR:
			perror("[ERROR]: Could not bind to socket");
			break;

		case SOCKET_LISTEN_ERROR:
			perror("[ERROR]: Could not listen to the socket");
			break;

		case SOCKET_CONNECTION_ERROR:
			perror("[ERROR]: Could not accept new connection");
			break;

		case SOCKET_SEND_ERROR:
			perror("[ERROR]: Could not send message");
			break;

		case SOCKET_RECEIVE_ERROR:
			perror("[ERROR]: Could not receive message");
			break;

		case SOCKET_HOST_ERROR:
			perror("[ERROR]: Could get host by address");
			break;

		default:
			perror("[ERROR]: Unidentified error has occurred");
			break;

	}

	return ERROR;
}//Done