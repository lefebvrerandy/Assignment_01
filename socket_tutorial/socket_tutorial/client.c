/*
*  FILE          : client.c
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains the functions and logic required to execute the client's required functionality.
*				   Functions are included for creating, connecting, and closing sockets, and for sending/receiving messages
*				   to and from the server
*/


#include "client.h"


/*
*  FUNCTION      : start_client_protocol
*  DESCRIPTION   : This function is used to create the client instance of the application. The client
*  PARAMETERS    : Parameters are as follows,
*	int stream_or_datagram : Denotes if the socket is of type SOCK_STREAM or SOCK_DGRAM
*	int tcp_or_udp		   : Denotes if the protocol is IPPROTO_TCP or IPPROTO_UDO
*  RETURNS       : int : Returns positive if the operation completed without error
*/
int start_client_protocol(int stream_or_datagram, int tcp_or_udp)
{

	int clientReturn = 0;											//Denotes if the function completed it's operation successfully 
	struct sockaddr_in socketAddress;								//Local address struct
	const char clientHostID[] = "client";							//Name of the client machine
	struct hostent* hostIdentifier;									//Represent an entry in the hosts database


	//Stage 1A: Get the client's network properties 
	memset((void*)&socketAddress, 0, sizeof(socketAddress));		//Clear the socket struct before initialization
	//socketAddress.sin_addr.s_addr = inet_addr(clientHostID);		//Convert the IPv4 internet host address name to binary
	//if (socketAddress.sin_addr.s_addr == INADDR_NONE)				//Check if the internet address field is valid (INADDR_NONE = error state of internet address) 
	//{

	//	hostIdentifier = gethostbyname(clientHostID);				//Returns a structure of type hostent for the given host name (contains either a host name or an IPv4 address)
	//	if (hostIdentifier == NULL)
	//	{
	//		clientReturn = setErrorState(SOCKET_HOST_ERROR);		//Unable to get host identifier (name or IP address)
	//	}
	//}
	//else
	//{
	//	hostIdentifier = gethostbyaddr((const char*)&socketAddress.sin_addr, sizeof(struct sockaddr_in), AF_INET);	//Returns a structure of type hostent for the given host name (contains either a host name or an IPv4 address)
	//	if (hostIdentifier == NULL)
	//	{
	//		clientReturn = setErrorState(SOCKET_HOST_ERROR);		//Unable to get host identifier (name or IP address)
	//	}
	//}



	//Stage 1B : Setup the client's address struct
	socketAddress.sin_family = AF_INET;											//Set the address family


	// TESTING
	socketAddress.sin_addr.s_addr = inet_addr(storedData[1]);
	//inet_aton(storedData[CLA_IP_ADDRESS], &socketAddress.sin_addr.s_addr);	//Set the IP address
	// TESTING END

	socketAddress.sin_port = htons(storedData[CLA_PORT_NUMBER]);				//Set the port
	

	//clientReturn = 1;

	//Host data has been retried and set, proceed to open the socket and run the message loop
	if (clientReturn != ERROR)
	{

		SOCKET openSocketHandle = createSocket(AF_INET, stream_or_datagram, tcp_or_udp);
		if (openSocketHandle == INVALID_SOCKET)
		{
			clientReturn = setErrorState(SOCKET_CREATION_ERROR);
		}
		else
		{

			//Connect to the server
			int boundSocketHandle = connectToServer(openSocketHandle, socketAddress);
			if (boundSocketHandle == SOCKET_ERROR)
			{
				clientReturn = setErrorState(SOCKET_CONNECTION_ERROR);
			}
			else
			{

				//Four possible block sizes are available for use
				char messageBuffer1000[MESSAGE_BUFFER_SIZE_1000] = {""};
				char messageBuffer2000[MESSAGE_BUFFER_SIZE_2000] = {""};
				char messageBuffer5000[MESSAGE_BUFFER_SIZE_5000] = {""};
				char messageBuffer10000[MESSAGE_BUFFER_SIZE_10000] = {""};


				clock_t startTime;
				clock_t endTime;
				double elapsedTime = 0;
				int sendStatus = 0;
				int currentblockCount = 0;


				//Convert the command line arguments and send the corresponding  block size and frequency
				int blockSize = convertCharToInt(storedData[CLA_BUFFER_SIZE]);
				int numberOfBlocks = convertCharToInt(storedData[CLA_NUMBER_OF_BLOCKS]);
				switch (blockSize)
				{
					case MESSAGE_BUFFER_SIZE_1000:

						//Prepare the outboundMessages for transmission, and fill each block with chars 0 - 9
						memset((void*)messageBuffer1000, 0, sizeof(messageBuffer1000));
						fillMessageBuffer(messageBuffer1000, MESSAGE_BUFFER_SIZE_1000);		


						//Start the timer, and send all the blocks across the network
						startTime = stopWatch();
						while (currentblockCount < numberOfBlocks)
						{
							sendStatus = sendMessage(openSocketHandle, messageBuffer1000);
						}
						endTime = stopWatch();
						elapsedTime = calculateElapsedTime(startTime, endTime);
						break;


					case MESSAGE_BUFFER_SIZE_2000:

						//Prepare the outboundMessages for transmission, and fill each block with chars 0 - 9
						memset((void*)messageBuffer2000, 0, sizeof(messageBuffer2000));
						fillMessageBuffer(messageBuffer2000, MESSAGE_BUFFER_SIZE_2000);


						//Start the timer, and send all the blocks across the network
						startTime = stopWatch();
						while (currentblockCount < numberOfBlocks)
						{
							sendStatus = sendMessage(openSocketHandle, messageBuffer2000);
						}
						endTime = stopWatch();
						elapsedTime = calculateElapsedTime(startTime, endTime);
						break;


					case MESSAGE_BUFFER_SIZE_5000:

						//Prepare the outboundMessages for transmission, and fill each block with chars 0 - 9
						memset((void*)messageBuffer5000, 0, sizeof(messageBuffer5000));
						fillMessageBuffer(messageBuffer5000, MESSAGE_BUFFER_SIZE_5000);


						//Start the timer, and send all the blocks across the network
						startTime = stopWatch();
						while (currentblockCount < numberOfBlocks)
						{
							sendStatus = sendMessage(openSocketHandle, messageBuffer5000);
						}
						endTime = stopWatch();
						elapsedTime = calculateElapsedTime(startTime, endTime);
						break;


					case MESSAGE_BUFFER_SIZE_10000:

						//Prepare the outboundMessages for transmission, and fill each block with chars 0 - 9
						memset((void*)messageBuffer10000, 0, sizeof(messageBuffer10000));
						fillMessageBuffer(messageBuffer10000, MESSAGE_BUFFER_SIZE_10000);


						//Start the timer, and send all the blocks across the network
						startTime = stopWatch();
						while (currentblockCount < numberOfBlocks)
						{
							sendStatus = sendMessage(openSocketHandle, messageBuffer10000);
						}
						endTime = stopWatch();
						elapsedTime = calculateElapsedTime(startTime, endTime);
						break;


					default:
						break;
				}


				//Prepare the buffer for incoming messages from the server
				char recievedBuffer[MESSAGE_BUFFER_SIZE_1000] = "";
				memset((void*)recievedBuffer, 0, sizeof(recievedBuffer));
				
				
				//Receive the messages informing the client about the missed, and disordered data from the last transmission
				clientReturn = receiveMessage(openSocketHandle, recievedBuffer);
				int proportionMissing = convertCharToInt(recievedBuffer);
				memset((void*)recievedBuffer, 0, sizeof(recievedBuffer));


				clientReturn = receiveMessage(openSocketHandle, recievedBuffer);
				int disordered = convertCharToInt(recievedBuffer);


				int totalBytes = blockSize * numberOfBlocks;
				int megaBitsPerSecond = calculateSpeed(totalBytes, (int)elapsedTime);


				printResults(blockSize, numberOfBlocks, (int)elapsedTime, megaBitsPerSecond, proportionMissing, disordered);	//Size: <<size>> Sent: <<sent>> Time: <<time>> Speed: <<speed>> Missing: <<missing>> Disordered: <<disordered>> 
			}
		}
		closesocket(openSocketHandle);
	}
	return clientReturn;
}

/*
*  FUNCTION      : connectToServer
*  DESCRIPTION   : This function is used to connect to the socket as defined by the arguments
*  PARAMETERS    : Parameters are as follows,
*	SOCKET openSocketHandle : The socket identifier which will be used to connect the client and server
*	struct sockaddr_in socketAddress : The socket struct containing the client's/socket properties
*  RETURNS       : int : Returns an integer denoting if the operation was completed successfully
*/
int connectToServer(SOCKET openSocketHandle, struct sockaddr_in socketAddress)
{
	int newBoundSocket = connect(openSocketHandle, (struct sockaddr*)&socketAddress, sizeof(struct sockaddr));
	return newBoundSocket;

}//Done


/*
*  FUNCTION      : fillMessageBuffer
*  DESCRIPTION   : This function is used to fill the messageBuffer with numbers 0 - 9
*  PARAMETERS    : Parameters are as follows,
*	int messageBuffer[] : Message buffer which will be filled with integers
*	int bufferSize		: Buffer size set outside of the function, and used to set the target loop count
*  RETURNS       : void : The function has no return value
*/
void fillMessageBuffer(char messageBuffer[], int bufferSize)
{
	int index = 0;
	int elementValue = 48;							//ASCII 48 is '0'
	for (index = 0; index < bufferSize; index++)	//Message buffer size can increase from 1000 - 10,000 bytes
	{
		if (elementValue < 58)
		{
			messageBuffer[index] = (char)elementValue;	//Fill the message buffer with the elements value from 0 - 9
			elementValue++;
		}
		else
		{
			elementValue = 48;						//Reset the value once it passes decimal 58 (ie. ascii '9' = (char)58)
		}
	}

}//Done


/*
*  FUNCTION      : stopWatch
*  DESCRIPTION   : This function is used to get the number of clock ticks since the process started. It returns the tick count
*				   and is used in conjunction with calculateElapsedTime() to calculate the total time required for an operation to complete
*  PARAMETERS    : void: The function takes no arguments
*  RETURNS       : clock_t : Returns the number of clock ticks since the process was started
*
*	NOTE: This function  was initially found online, however, the original source code has since been modified to suit the projects needs.
*		   As a result, partial credit belongs to the original authors on the website. For more information, please see the reference,
*		   GeeksForGeeks.(ND). How to measure time taken by a function in C?. Retrieved on January 8, 2019,
*			from https://www.geeksforgeeks.org/how-to-measure-time-taken-by-a-program-in-c/
*/
clock_t stopWatch(void)
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


/*
*  FUNCTION      : convertCharToInt
*  DESCRIPTION   : This function is used to convert characters to an integer
*  PARAMETERS    : char* stringToConvert : The character string that will be converted to its integer counterpart
*  RETURNS       : int : Returns the converted number from the character array
*/
int convertCharToInt(char* stringToConvert)
{
	int returnNumber = 0;
	sscanf(stringToConvert, "%d", &returnNumber);
	return returnNumber;

}//Done


/*
*  FUNCTION      : calculateSpeed
*  DESCRIPTION   : This function is used to calculate the speed to transmission between the client and the server, by converting it into megabits per second
*  PARAMETERS    : parameters are as follows, 
*	int bytes		  : Total number of bytes sent to the server
*	int elapsedTimeMS : Total elapsed time (ms) needed to send all the bytes to the server
*  RETURNS       : int : Returns the speed of transmission as megabits / second
*/
int calculateSpeed(int bytes, int elapsedTimeMS)
{
	int speed = 0;

	//Speed is calculated as (megabytes / sec) * 8 (ie. megabits)
	speed = ((bytes * 1024) / elapsedTimeMS) * 8;

	return speed;

}//DOne


/*
*  FUNCTION      : printResults
*  DESCRIPTION   : This function is used to print the results of the client-server interaction
*  PARAMETERS    : Parameters are as follows,
*	int size : The size of the blocks used during the sending procedure
*	int sent : The number of blocks sent
*	int time : The time taken to send the blocks
*	int speed : The speed of message transmission
*	int missing : The count of messaged that were missing
*	int disordered : The count of messages that were disordered
*  RETURNS       : void  : Function has no return value
*/
void printResults(int size, int sent, int time, int speed, int missing, int disordered)
{
	printf("Size: %d Sent: %d, Time: %d, Speed: %d, Missing: %d, Disordered: %d", size, sent, time, speed, missing, disordered);
}