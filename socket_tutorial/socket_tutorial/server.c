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
		memset((void*)&socketAddress, 0, sizeof(socketAddress));					//Clear the address struct for initialization
		socketAddress.sin_family = AF_INET;											//Address family internet protocol
		socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);							//Convert from host byte order to network byte order
		socketAddress.sin_port = htons((u_short)(storedData[CLA_PORT_NUMBER]));		//Port defined by CLA


		//Convert an Internet network address into ASCII string 
		char hostbuffer[256];
		struct hostent *host_entry;
		host_entry = gethostbyname(hostbuffer);
		char *IPbuffer;
		IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));


		//Print server connection info to the screen
		printf("Hostname: %s\n", hostbuffer);					//DEBUG REMOVE BEFORE SUBMISSION
		printf("Host IP: %s\n", IPbuffer);
		printf("Port: %s\n", storedData[CLA_PORT_NUMBER]);


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

					//SELECT() tutorial https://www.youtube.com/watch?v=qyFwGyTYe-M
					struct timeval timeout;					//Tracks socket connection details
					int socketDescriptor = 0;				//File descriptor (ie. target socket)
					fd_set readfds;							//file descriptor to be checked for being ready to read
					FD_ZERO(&readfds);						//Initializes the file descriptor by setting fdset to have zero bits for all file descriptors
					FD_SET(socketDescriptor, &readfds);		//Sets the bit for the file descriptor fd in the file descriptor


					int sret = select(8, &readfds, NULL, NULL, &timeout);
					if (sret == 0)
					{
						//Operation timedout

					}
					else
					{
						//Operation is not done
					}

					//Stage 6: Receive the clients reply
					char messageBuffer[MESSAGE_BUFFER_SIZE_10000] = {""};
					recv(acceptedSocketConnection, messageBuffer, sizeof(messageBuffer), 0);
					int amountOfTimesReceived = 1;


					//Make a copy of the original message
					char messageCopy[MESSAGE_BUFFER_SIZE_10000] = {""};
					strcpy(messageCopy, messageBuffer);


					MessageProperties protocol;		//Tracks message properties
					NetworkResults messageData;		//Tracks communication results
					

					//Initialize the struct
					messageData.prevBlockID = 0;
					messageData.currentBlockID = 0;
					messageData.bytesReceived = 0;
					messageData.missingBytes = 0;
					messageData.missingBlocks = 0;
					messageData.disorganizedBytes = 0;


					//Deconstruct the message and get its properties
					protocol.blockSize = getBlockSize(messageCopy);
					protocol.blockCount = getNumberOfBlocks(messageCopy);

					do
					{

						//Get the blocks ID and compare it to the previous one to see if any were missed
						messageData.currentBlockID = getBlockID(messageBuffer);
						messageData.missingBlocks += checkForMissedBlock(messageData.currentBlockID, messageData.prevBlockID);
						messageData.missingBytes = getBytesMissing(protocol.blockSize, messageBuffer);


						//Find where the real message starts denoted by the letter 'G'
						char *ptr = NULL;
						ptr = strchr(messageBuffer, 'G');
						ptr++;
						strcpy(messageBuffer, ptr);



						//Clear the buffer and receive the next message if another one is still expected
						if (amountOfTimesReceived < protocol.blockCount)
						{
							memset((void*)messageBuffer, 0, sizeof(messageBuffer));
							recv(acceptedSocketConnection, messageBuffer, sizeof(messageBuffer), 0);
							amountOfTimesReceived++;
						}
					}while (amountOfTimesReceived < protocol.blockCount);
				

					int totalReceivedBytes = messageData.bytesReceived * amountOfTimesReceived;
					int totalExpectingReceivedBytes = protocol.blockSize * 0;
					int totalLostBytes = totalExpectingReceivedBytes - totalReceivedBytes;
					printf("Amount of Bytes Received: %d\n", totalReceivedBytes);
					printf("Amount of Bytes Sent From Client: %d\n", totalExpectingReceivedBytes);
					printf("Amount of Lost Bytes: %d\n", totalLostBytes);
					printf("\n");
				}
			}
		}
	}


	//Clean up before exiting
	closesocket(acceptedSocketConnection);
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
	for (int index = 0; index < BLOCK_SIZE_LENGTH; index++)								
	{
		messageProperties[index] = messageCopy[index];				//String stating the block size will never be larger than 4 chars (ie. first 4 chars of the array)
	}

	//Get the decimal value from the string 
	long blockSize = convertHexToDecimal(messageProperties);
	free(messageProperties);
	return blockSize;
}


/*
*  FUNCTION      : convertHexToDecimal
*  DESCRIPTION   : This method is used to DEBUG
*  PARAMETERS    : Function parameters are as follows
*  RETURNS       : int : Returns an integer indicating the functions success (ie. return > 0) or failure (ie. return < 0)
*/
int convertHexToDecimal(char* messageProperties)
{
	int convertedHex = 0;
	int val = 0;
	int expectingBytes = 0;
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

		expectingBytes += val * (long)(pow(16, len));
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
	for (int i = 0; ptr[i] != 'G'; i++)
	{

		//Copy each element of the block count string
		blockCountArray[i] = messageCopy[i + BLOCK_SIZE_OFFSET];
	}
	int blockCount = 0;
	sprintf(blockCount, "%d", blockCountArray);
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


	//Jump to the blockID identifier in the message


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