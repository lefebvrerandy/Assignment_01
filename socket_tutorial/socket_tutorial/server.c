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
		printf("Could not create Thread.");
	}
	else if (pthread_create(&thread_linux_server[1], NULL, start_server_protocol, (void*)udpArray) != 0)
	{
		// An error has occurred
		printf("Could not create Thread.");
	}

	for (int i = 0; i < 2; i++)
	{
		if (pthread_join(thread_linux_server[i], (void **)ptr_status) != 0)
		{
			printf("Cannot join Threads.");
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
							int recvStatus = 0;
							do
							{
								recvStatus = recv(acceptedSocketConnection, messageBuffer, sizeof(messageBuffer), 0);
							} while (recvStatus < 0);
							int amountOfTimesReceived = 1;


							//Make a copy of the original message
							char messageCopy[MESSAGE_BUFFER_SIZE_10000] = { "" };
							strcpy(messageCopy, messageBuffer);


							//Initialie the struct for tracking communication results
							NetworkResults messageData; messageData.prevBlockID = 0; messageData.currentBlockID = 0;
							messageData.bytesReceivedCount = 0; messageData.blocksReceivedCount = 0;  messageData.missingBytesCount = 0;
							messageData.missingBlockCount = 0; messageData.disorganizedBlocksCount = 0; messageData.disorganizedBytesCount = 0;


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
								messageData.missingBytes = getBytesMissing(protocol.blockSize, messageBuffer);


								messageData.blocksReceivedCount = getBlockCount(messageData.receivedBlocks);
								messageData.missingBytesCount = getBytesMissing(protocol.blockSize, messageBuffer);


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


							//Ensure the socket is still intact before sending the message
							if (networkResult == SOCKET_TIMEOUT)
							{
								//Package the results and send them off to the client and disconnect the socket
								packageResults(messageBuffer, messageData.missingBlockCount);
								send(acceptedSocketConnection, messageBuffer, strlen(messageBuffer), 0);
								packageResults(messageBuffer, messageData.disorganizedBlocksCount);
								send(acceptedSocketConnection, messageBuffer, strlen(messageBuffer), 0);
							}


							//Report the server size details DEBUG REMOVE BEFORE SUBMISSION
							printf("Amount of Bytes Sent From Client: %d\n", (int)(protocol.blockCount * protocol.blockSize));
							printf("Amount of Bytes Received: %d\n", messageData.bytesReceivedCount);
							printf("Amount of Blocks Received: %d\n", messageData.bytesReceivedCount);
							printf("Amount of Lost Blocks: %d\n", messageData.missingBlockCount);
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
*  DESCRIPTION   : This method is used to get the block size from the message header,
*					and convert it to a single decimal value
*  PARAMETERS    : Function parameters are as follows
*	char messageCopy[] : Copy of the message array
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

}//Done


/*
*  FUNCTION      : convertHexToDecimal
*  DESCRIPTION   : This method is used to convert a hexadecimal string fo characters into a single integer of equivilent value
*  PARAMETERS    : Function parameters are as follows
*	char messageProperties : Pointer to the string containing the message proerties
*  RETURNS       : int : Returns an integer indicating the functions success (ie. return > 0) or failure (ie. return < 0)
*/
int convertHexToDecimal(char* messageProperties)
{
	int convertedHex = 0;
	sscanf(messageProperties, "%x", &convertedHex);
	return (long)convertedHex;

}//Done


/*
*  FUNCTION      : getNumberOfBlocks
*  DESCRIPTION   : This method is used to get the number of expected blocks from the message header, and convert them from
*				   hex to decimal form
*  PARAMETERS    : Function parameters are as follows
*	char messageCopy[] : Copy of the message array
*  RETURNS       : int : Returns an integer indicating the functions success (ie. return > 0) or failure (ie. return < 0)
*/
int getNumberOfBlocks(char messageCopy[])
{
	char blockCountArray[MESSAGE_PROPERTY_SIZE] = { "" };


	//Scan the remainder of the string until the letter 'G' is encountered
	int i = BLOCK_SIZE_OFFSET;
	int j = 0;
	for (i = BLOCK_SIZE_OFFSET; i < 8; i++)
	{
		blockCountArray[i] = messageCopy[i + BLOCK_SIZE_OFFSET];	//Block size will always be from index 4 to 7

		//Copy each element of the block count string
		blockCountArray[j] = messageCopy[i];
		j++;
	}
	blockCountArray[j] = '\0';

	int blockCount = 0;
	blockCount = convertHexToDecimal(blockCountArray);
	return blockCount;

}//Done


/*
*  FUNCTION      : getBlockID
*  DESCRIPTION   : This method is used to find the block ID in the message header, and convert the hex value to a decimal form
*  PARAMETERS    : Function parameters are as follows
*	char messageCopy[] : Copy of the message array
*  RETURNS       : int : Returns an integer indicating the functions success (ie. return > 0) or failure (ie. return < 0)
*/
int getBlockID(char messageCopy[])
{
	char blockIDSubSet[MESSAGE_PROPERTY_SIZE] = { "" };

	//Copy each element of the block count string
	for (int i = 0; i < 4; i++)
	{
		blockIDSubSet[i] = messageCopy[i + BLOCK_ID_OFFSET];	//Block ID will always be from index 8 - 11

	}

	//Convert the hex string representing the block ID, and return the result
	int blockID = convertHexToDecimal(blockIDSubSet);
	return blockID;

}//Done


/*
*  FUNCTION      : getBlocksReceived
*  DESCRIPTION   : This method is used to DEBUG
*  PARAMETERS    : Function parameters are as follows
*	char messageCopy[] : Copy of the message array
*	DEBUG
*  RETURNS       : int : Returns an integer indicating the functions success (ie. return > 0) or failure (ie. return < 0)
*/
int getBlocksReceived(char messageCopy[], NetworkResults blockCounter)
{
	int blockCount = 0;



	return blockCount;
}


/*
*  FUNCTION      : checkForMissedBlock
*  DESCRIPTION   : This method is used to check if the block's ID has been added to the list of blocks that are missing
*  PARAMETERS    : Function parameters are as follows,
*	int currentBlockID		: Block ID of the most recent block
*	int prevBlockID			: ID of the previously received block
*	int receivedBlockList[] : Array containing the ID's of all the blocks that have been recieved
*	int missingBlocks[]			: Array containing the ID's of all the blocks listed as missing
*	int disorganizedBlockList[] : Array containing the ID's of all the blocks that have been labled as disorganized
*  RETURNS       : void : The function has no return
*/
void checkForMissedBlock(int currentBlockID, int prevBlockID, int receivedBlockList[], int missingBlockList[], int disorganizedBlockList[])
{
	int missingBlocks = 0;
	if (currentBlockID == prevBlockID + 1)
	{
		addBlockToRecievedList(currentBlockID, receivedBlockList);

	}	//Sequential block transmission was accurate
	else//BlockID did not match the expected ID
	{

		//Check if the block is labeled as missing, and add it to the list if found
		bool missingBlock = isBlockMissing(currentBlockID, missingBlockList);
		if (missingBlock == true)
		{

			//Block was listed as missing; add to disorganized list
			addBlockTodisorganizedList(currentBlockID, disorganizedBlockList);
		}
	}

}//Done


/*
*  FUNCTION      : isBlockMissing
*  DESCRIPTION   : This method is used to check if the block's ID has been added to the list of missing blocks, if true, 
*				   remove the ID from the list and return true
*  PARAMETERS    : Function parameters are as follows,
*	int currentBlockID  : Block ID of the most recent block
*	int missingBlocks[] : Array containing the ID's of all the blocks listed as missing
*  RETURNS       : bool : Returns true if the block's ID was found in the list
*/
bool  isBlockMissing(int currentBlockID, int missingBlocks[])
{
	bool missingBlockFound = false;


	//Check if the new block is an old block that was missed, 
	// or if the block was sent too early
	for (int index = 0; index < strlen(missingBlocks); index++)
	{
		if (missingBlocks[index]== currentBlockID)
		{
			missingBlockFound = true;	//Block was found
			missingBlocks[index] = 0;	//Remove the blockID from the list of missing ID's
		}

	}

	return missingBlocks;
}


/*
*  FUNCTION      : addBlockTodisorganizedList
*  DESCRIPTION   : This method is used to check if the block's ID has been added to the list of blocks that came in after they were labeled as missing;
*					In other words, they will be labeled as disorganized, and removed from the missingBlockList
*  PARAMETERS    : Function parameters are as follows,
*	int currentBlockID  : Block ID of the most recent block
*	int missingBlocks[] : Array containing the ID's of all the blocks listed as missing
*  RETURNS       : bool : Returns true if the block's ID was found in the list
*/
void addBlockTodisorganizedList(int blockID, int disorganizedList[])
{
	disorganizedList
}


/*
*  FUNCTION      : getBytesMissing
*  DESCRIPTION   : This function is used to get the number of bytes missing from the message.
*				   The function compares the total message bytes count against the expected block size
*  PARAMETERS    : Function parameters are as follows,
*	int blockSize		: Block size indicator (1000 to 10,000)
*	char* messageBuffer : Original message passed from the client
*  RETURNS       : int : Returns an integer indicating the number of bytes lost during transmission
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

}//Done 


/*
*  FUNCTION      : packageResults
*  DESCRIPTION   : This method is used to DEBUG
*  PARAMETERS    : Function parameters are as follows,
*	char messagBuffer[] : Outbounnd message container
*	int packagedValue   : Int value to be added to the message
*  RETURNS       : void : Function has no return
*/
void packageResults(char messagBuffer[], int packagedValue)
{
	sprintf(messagBuffer, "%d", packagedValue);

}//Done