/*
*  FILE          : server.c
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains all the functions required to operate the sever component of the application.
*				   Functions are included for adding multi thread support, opening TCP & UDP sockets, and sending/recieving 
*				   messages from the clients
*/


#include "server.h"


/*
*  FUNCTION      : start_server
*  DESCRIPTION   : The function is used to create two threads for the server, in order to create a UDP and TCP socket 
*				   for each instance of the server
*  PARAMETERS    : void : This function takes no arguments
*  RETURNS       : int : Returns constant zero indicating the conditional server threading was completed without serious error
*/
int start_server()
{
	int tcpArray[2] = { {SOCK_STREAM},{IPPROTO_TCP} };
	int udpArray[2] = { {SOCK_DGRAM}, {IPPROTO_UDP} };
	//Spawn two threads. One for TCP, one for UDP
#if defined _WIN32
	HANDLE thread_windows_server[2];

	thread_windows_server[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)start_server_protocol, (LPVOID)tcpArray, 0, NULL);
	//thread_windows_server[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)start_server_protocol, (LPVOID)udpArray, 0, NULL);

	WaitForMultipleObjects(2, thread_windows_server, TRUE, INFINITE);
	Sleep(1000000);
	for (int i = 0; i < 2; i++)
	{
		CloseHandle(thread_windows_server[i]);
	}

#elif defined __linux__
	pthread_t thread_linux_server[2];
	if (pthread_create(&thread_linux_server[0], NULL, start_server_protocol, (void*)tcpArray) != 0)
	{
		// An error has occurred
		perror("Could not create Thread.");
	}
	else if (pthread_create(&thread_linux_server[1], NULL, start_server_protocol, (void*)udpArray) != 0)
	{
		// An error has occurred
		perror("Could not create Thread.");
	}

	for (int i = 0; i < 2; i++)
	{
		if (pthread_join(thread_linux_server[i], (void **)ptr_status) != 0)
		{
			perror("Cannot join Threads.");
		}
	}

#endif

	return 0;
}


/*
*  FUNCTION      : start_server_protocol
*  DESCRIPTION   : This method is used to create, bind, listen, connect, and receive across standard socket for the server. 
*				   The function acts as a high level controller by calling each of the required functions and setting the properties 
*				   of the socket based on the parameters provided by the user from the command line
*  PARAMETERS    : Function parameters are as follows
*	int stream_or_datagram : Denotes if the socket is of type SOCK_STREAM or SOCK_DGRAM
*	int tcp_or_udp		   : Denotes if the protocol is IPPROTO_TCP or IPPROTO_UDO
*  RETURNS       : int : Returns an integer indicating the functions success (ie. return > 0) or failure (ie. return < 0)
*/
int start_server_protocol(int* tcpOrUdp)
{
	int networkResult = 1;				//Denotes the success or failure of the servers operation
	SOCKET acceptedSocketConnection;	//Socket used for connecting to the clients


	//Stage 1: Create local socket
	SOCKET openSocketHandle = createSocket(AF_INET, tcpOrUdp[0], tcpOrUdp[1]);
	if (openSocketHandle == ERROR_RETURN)
	{
		networkResult = setErrorState(SOCKET_CREATION_ERROR);						//Set return to -1, and print an error for the stage of connection
	}
	else
	{

		//Stage 2A: Initialize the socket struct
		struct sockaddr_in socketAddress;
		memset((void*)&socketAddress, 0, sizeof(socketAddress));				//Clear the address struct for initialization
		socketAddress.sin_family = AF_INET;										//Address family internet protocol
		socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);						//Convert from host byte order to network byte order
		socketAddress.sin_port = htons((u_short)storedData[CLA_PORT_NUMBER]);
		

		char hostbuffer[HOST_BUFFER_SIZE];
		char *IPbuffer;
		struct hostent *host_entry;
		int hostname;
		char hostPort[PORT_LENGTH];
		strcpy(hostPort, storedData[CLA_PORT_NUMBER]);
		hostname = gethostname(hostbuffer, sizeof(hostbuffer));
		host_entry = gethostbyname(hostbuffer);
		IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));


		//Print the servers details
		printf("Hostname: %s\n", hostbuffer);
		printf("Host IP: %s\n", IPbuffer);
		printf("Port: %s\n", hostPort);


		//Stage 2B: Bind to the open socket
		int boundSocketHandle = bind(openSocketHandle, (struct sockaddr*)&socketAddress, sizeof(socketAddress));
		if (boundSocketHandle == SOCKET_ERROR)
		{
			networkResult = setErrorState(SOCKET_BIND_ERROR);					//Set return to -1, and print an error for the stage of connection
		}
		else
		{

			//Stage 3: Listen for an incoming connection to the open socket
			boundSocketHandle = listen(openSocketHandle, SOMAXCONN);
			if (boundSocketHandle == SOCKET_ERROR)
			{
				networkResult = setErrorState(SOCKET_LISTEN_ERROR);				//Set return to -1, and print an error for the stage of connection
			}
			else
			{
				do
				{
					//Stage 4: Accept the incoming client connection
					struct sockaddr_in remoteAddress;
					socklen_t addressSize = sizeof(remoteAddress);
					acceptedSocketConnection = accept(openSocketHandle, (struct sockaddr*)&remoteAddress, &addressSize);
					if (acceptedSocketConnection == INVALID_SOCKET)
					{
						networkResult = setErrorState(SOCKET_CONNECTION_ERROR);		//Set return to -1, and print an error for the stage of connection
					}
					else
					{
						struct timeval timeout;					//Tracks socket connection details
						timeout.tv_sec = 5;						//Timeout to 5 seconds
						timeout.tv_usec = 0;					//Timeout to 0 milliseconds


						//Set the socket to be non blocking and have a time out as expressed above 
						if (setsockopt(acceptedSocketConnection, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
						{
							networkResult = setErrorState(SOCKET_SETTINGS_ERROR);		//Set return to -1, and print an error for the stage of connection
						}
						else
						{

							//Stage 6: Receive the clients reply
							char messageBuffer[MESSAGE_BUFFER_SIZE_10000] = { "" };
							int recvStatus = recv(acceptedSocketConnection, messageBuffer, sizeof(messageBuffer), 0);
							int amountOfTimesReceived = 1;


							//Make a copy of the original message
							char messageCopy[MESSAGE_BUFFER_SIZE_10000] = { "" };
							strcpy(messageCopy, messageBuffer);


							//Initialie the struct for tracking communication results
							NetworkResults messageData;
							messageData.prevBlockID = 0;
							messageData.currentBlockID = 0;
							messageData.bytesReceived = 0;
							messageData.missingBytes = 0;
							messageData.missingBlocks = 0;
							messageData.disorganizedBytes = 0;


							//Deconstruct the message and get its properties
							MessageProperties protocol;									//Tracks message properties
							protocol.blockSize = getBlockSize(messageCopy);
							protocol.blockCount = getNumberOfBlocks(messageCopy);
							int selectReturn = 0;
							do
							{
								//Get the blocks ID and compare it to the previous one to see if any were missed
								messageData.currentBlockID = getBlockID(messageBuffer);
								messageData.missingBlocks += checkForMissedBlock(messageData.currentBlockID, messageData.prevBlockID);
								messageData.prevBlockID = messageData.currentBlockID;
								messageData.missingBytes = getBytesMissing(protocol.blockSize, messageBuffer);


								//Find where the real message starts denoted by the letter 'G'
								//char *ptr = NULL;
								//ptr = strchr(messageBuffer, 'G');
								//ptr++;
								//strcpy(messageBuffer, ptr);	//DEBUG MIGHT NEED TO CHANGE THIS DEPENDING ON HOW RANDY SETS THE CLIENT UP


								//Clear the buffer and receive the next message if another one is still expected
								memset((void*)messageBuffer, 0, sizeof(messageBuffer));
								recvStatus = recv(acceptedSocketConnection, messageBuffer, sizeof(messageBuffer), 0);


								//recv() call returned zero bytes indicating the connection was closed
								if (recvStatus == 0)
								{
									networkResult = setErrorState(SOCKET_CLOSED);
								}

								//recv() call returned an error indicating the connection timedout
				// Double check this else
								else
								{
									networkResult = setErrorState(SOCKET_TIMEOUT);
								}
								amountOfTimesReceived++;
							} while (recvStatus > 0);


							//Package the results and send them off to the client and disconnect the socket
							//packageResults(messageBuffer, 
							//	messageData,
							//	messageData.missingBlocks);


							//Ensure the socket is still intact before sending the message
							if (networkResult == SOCKET_TIMEOUT)
							{
								send(acceptedSocketConnection, messageBuffer, strlen(messageBuffer), 0);
							}


							//Report the server size details DEBUG REMOVE BEFORE SUBMISSION
							printf("Amount of Bytes Sent From Client: %d\n", (int)(protocol.blockCount * protocol.blockSize));
							printf("Amount of Bytes Received: %d\n", messageData.bytesReceived);
							printf("Amount of Lost Blocks: %d\n", messageData.missingBlocks);
							printf("\n");
						}
					}

					closesocket(acceptedSocketConnection);
				} while (1);	//DEBUG SET TO MONITOR TIME AND AUOT DISCONNECT ONCE A CERTAIN TIME SPAN HAS PASSED WITH NO INCOMING CONNECTIONS
			}
		}
	}


	//Clean up before exiting
	closesocket(openSocketHandle);
	return networkResult;
}


/*
*  FUNCTION      : getBlockSize
*  DESCRIPTION   : This method is used to DEBUG
*  PARAMETERS    : Function parameters are as follows
*  RETURNS       : int : Returns an integer indicating the functions success (ie. return > 0) or failure (ie. return < 0)
*/
long getBlockSize(char messageCopy[])
{
	//Get the block size sub string from the message
	char* messageProperties = malloc(sizeof(char) * MESSAGE_PROPERTY_SIZE);
	int index = 0;
	for (index = 0; index < BLOCK_SIZE_LENGTH; index++)								
	{
		messageProperties[index] = messageCopy[index];				//String stating the block size will never be larger than 4 chars (ie. first 4 chars of the array)
	}
	messageProperties[index] = '\0';


	//Get the decimal value from the string 
	long blockSize = convertHexToDecimal(messageProperties);
	free(messageProperties);
	return blockSize;
}


/*
*  FUNCTION      : convertHexToDecimal
*  DESCRIPTION   : 
*  PARAMETERS    : Function parameters are as follows
*  RETURNS       : int : Returns an integer indicating the functions success (ie. return > 0) or failure (ie. return < 0)
*/
int convertHexToDecimal(char* messageProperties)
{
	int convertedHex = 0;
	int val = 0;
	int len = strlen(messageProperties) - 1;
	for (int index = 0; index < 4; index++)
	{
		if (messageProperties[index] >= '0' && messageProperties[index] <= '9')
		{
			val = messageProperties[index] - 48;
		}
		else if (messageProperties[index] >= 'a' && messageProperties[index] <= 'f')
		{
			val = messageProperties[index] - 97 + 10;
		}
		else if (messageProperties[index] >= 'A' && messageProperties[index] <= 'F')
		{
			val = messageProperties[index] - 65 + 10;
		}

		convertedHex += val * (long)(pow(16, len));
		len--;
	}

	return (long)convertedHex;
}


/*
*  FUNCTION      : getNumberOfBlocks
*  DESCRIPTION   : This method is used to DEBUG
*  PARAMETERS    : Function parameters are as follows
*  RETURNS       : int : Returns an integer indicating the functions success (ie. return > 0) or failure (ie. return < 0)
*/
int getNumberOfBlocks(char messageCopy[])
{
	char blockCountArray[MESSAGE_BUFFER_SIZE_10000] = { "" };
	char *ptr = NULL;
	ptr = messageCopy;								//Point to the beginning of the copied message
	ptr += BLOCK_SIZE_OFFSET;						//Offset the index by 4 (block size string will always be the first 4 chars in the message)


	//Scan the remainder of the string until the letter 'G' is encountered
	int i = BLOCK_SIZE_OFFSET;
	int j = 0;
	for (i = BLOCK_SIZE_OFFSET; i < 8; i++)
	{

		//Copy each element of the block count string
		blockCountArray[j] = messageCopy[i];
		j++;
	}
	blockCountArray[j] = '\0';

	int blockCount = 0;
	blockCount = convertHexToDecimal(blockCountArray);
	return blockCount;
}


/*
*  FUNCTION      : getBlockID
*  DESCRIPTION   : This method is used to DEBUG
*  PARAMETERS    : Function parameters are as follows
*  RETURNS       : int : Returns an integer indicating the functions success (ie. return > 0) or failure (ie. return < 0)
*/
int getBlockID(char messageCopy[])
{
	int blockID = 0;
	char blockIdArray[5] = {""};

	//Jump to the blockID identifier in the message

	int i = 8;
	int j = 0;
	for (i = 8; i < 12; i++)
	{

		//Copy each element of the block id string
		blockIdArray[j] = messageCopy[i];
		j++;
	}

	blockID = convertHexToDecimal(blockIdArray);


	return blockID;
}


/*
*  FUNCTION      : checkForMissedBlock
*  DESCRIPTION   : This method is used to DEBUG
*  PARAMETERS    : Function parameters are as follows
*  RETURNS       : int : Returns an integer indicating the functions success (ie. return > 0) or failure (ie. return < 0)
*/
int  checkForMissedBlock(int currentBlockID, int prevBlockID)
{
	int missingBlocks = 0;
	if (currentBlockID != prevBlockID + 1)
	{
		//A block was missed, so increment the counter
		missingBlocks++;
	}

	return missingBlocks;
}


/*
*  FUNCTION      : getBytesMissing
*  DESCRIPTION   : This method is used to DEBUG
*  PARAMETERS    : Function parameters are as follows
*  RETURNS       : int : Returns an integer indicating the functions success (ie. return > 0) or failure (ie. return < 0)
*/
int getBytesMissing(int blockSize, char* messageBuffer)
{
	int lostBytes = 0;


	//Compare bytes received to what was expected
	int bytesReceived = strlen(messageBuffer);
	if (bytesReceived != blockSize)
	{
		lostBytes += blockSize - bytesReceived;
	}

	return lostBytes;
}