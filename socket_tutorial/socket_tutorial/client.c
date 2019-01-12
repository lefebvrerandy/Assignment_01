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
	memset((void*)&socketAddress, 0, sizeof(socketAddress));		//Clear the socket struct before initialization


	//Stage 1A : Setup the client's address struct
	socketAddress.sin_family = AF_INET;											
	socketAddress.sin_addr.s_addr = inet_addr(storedData[CLA_IP_ADDRESS]);
	socketAddress.sin_port = htons((u_short)(storedData[CLA_PORT_NUMBER]));


	//Host data has been retried and set, proceed to open the socket and run the message loop
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

			//Convert the CLA's and allocate space for the message buffer
			int numberOfBlocks = convertCharToInt(storedData[CLA_NUMBER_OF_BLOCKS]);
			int blockSize = convertCharToInt(storedData[CLA_BUFFER_SIZE]);
			int totalBytes = blockSize * numberOfBlocks;
			char* messageBuffer = CreateMessageBuffer(blockSize);


			#if defined _WIN32
			Timer stopwatch;
			stopwatch.startTime = GetTickCount();				//Start the Windows timer

			#elif defined __linux__
			startTime = stopWatch();							//Start the UNIX timer

			#endif
			int currentblockCount = 0;
			while (currentblockCount < numberOfBlocks)			
			{
				sendMessage(openSocketHandle, messageBuffer);	//Send the blocks across the network
				currentblockCount++;
			}

			#if defined _WIN32
			stopwatch.endTime = GetTickCount();					//Stop the Windows timer

			#elif defined __linux__
			endTime = stopWatch();								//Stop the UNIX timer

			#endif
			stopwatch.elapsedTime = stopwatch.endTime - stopwatch.startTime;


			//Receive message from server about the missed data
			memset((void*)messageBuffer, 0, sizeof(messageBuffer));
			recv(openSocketHandle, messageBuffer, sizeof(messageBuffer), 0);
			int proportionMissing = convertCharToInt(messageBuffer);


			//Receive message from server about the disordered data from the last transmission
			memset((void*)messageBuffer, 0, sizeof(messageBuffer));
			recv(openSocketHandle, messageBuffer, sizeof(messageBuffer), 0);
			int disordered = convertCharToInt(messageBuffer);


			int megaBitsPerSecond = calculateSpeed(totalBytes, (int)stopwatch.elapsedTime);


			printResults(blockSize, numberOfBlocks, (int)stopwatch.elapsedTime, megaBitsPerSecond, proportionMissing, disordered);	//Size: <<size>> Sent: <<sent>> Time: <<time>> Speed: <<speed>> Missing: <<missing>> Disordered: <<disordered>> 
			free(messageBuffer);
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
*  RETURNS       : int : Returns an integer denoting if the operation was completed successfully
*/
int connectToServer(SOCKET openSocketHandle, struct sockaddr_in socketAddress)
{
	int newBoundSocket = connect(openSocketHandle, (struct sockaddr*)&socketAddress, sizeof(struct sockaddr));
	return newBoundSocket;

}//Done


/*
*  FUNCTION      : CreateMessageBuffer
*  DESCRIPTION   : This function is used to reserve a chunk of space in memory with the size 
*				   defined at run time by the CLA's
*  PARAMETERS    : Parameters are as follows,
*	int bufferSize : Size of memory to reserve for the message buffer
*  RETURNS       : void : The function has no return value
*/
char* CreateMessageBuffer(int bufferSize)
{
	//Prepare the outboundMessages for transmission, and fill each block with chars 0 - 9
	char *returnArray = malloc(sizeof(char) * bufferSize);
	fillMessageBuffer(returnArray, bufferSize, storedData[CLA_NUMBER_OF_BLOCKS]);
	
	return returnArray;

}//Done


/*
*  FUNCTION      : fillMessageBuffer
*  DESCRIPTION   : This function is used to fill the messageBuffer with numbers 0 - 9
*  PARAMETERS    : Parameters are as follows,
*	int messageBuffer[] : Message buffer which will be filled with integers
*	int bufferSize		: Buffer size set outside of the function, and used to set the target loop count
*	char numOfTimes[]	: The number of messages that will be passed to the server
*  RETURNS       : void : The function has no return value
*/
void fillMessageBuffer(char messageBuffer[], int bufferSize, char numOfTimes[])
{
	int index = 0;
	int elementValue = 48;							//ASCII 48 is '0'
	char tempStartingPoint[255] = { "" };

	// Insert some data at the beginning of the message buffer so that the server knows what will all be sent
	char sizeOfBuffInHex[4][4] = { {"03E8"},{"07D0"},{"1388"},{"2710"} };
	int indexOfHexToInsert = 0;
	const int hexLength = 4;	// This will be used to find out the total of characters already added
	switch (bufferSize)
	{
		case 1000:
			indexOfHexToInsert = 0;
			break;
		case 2000:
			indexOfHexToInsert = 1;
			break;
		case 5000:
			indexOfHexToInsert = 2;
			break;
		case 10000:
			indexOfHexToInsert = 3;
			break;
	}

	// //////////////////////////////////////////////////////////////////////////////
	// 
	// From here one out, we are going to fill the buffer one index at a time. 
	// The start to each of the messages will contain 5 very important character,
	//	possible alittle more:
	//		The first 4 characters are a hexadecimal converting number that contains
	//		the block size that will be sent.
	//		The fifth character, and until the first "-" will be the amount of messages
	//		that will be passed.
	// An example would be :"03E810G"
	//			This will be parsed on the server to read "03E8" = 1000 size and "10" = times
	//
	// //////////////////////////////////////////////////////////////////////////////

	// Insert the Hex value for the buffer that is being sent to the server at the beginning of every message
	int i = 0;
	for (i = 0; i < 4; i++)
	{
		tempStartingPoint[i] = sizeOfBuffInHex[indexOfHexToInsert][i];
	}

	// Insert the number of messages that will be passed to the server
	int lengthOfNumOfTimes = strlen(numOfTimes);
	int indexOfTimes = 0;
	int j = 4;
	for (j = 4; j < lengthOfNumOfTimes + 4; j++)
	{
		tempStartingPoint[j] = numOfTimes[indexOfTimes];
		indexOfTimes++;
	}
	
	tempStartingPoint[j] = 'G';

	// Finally...Add the starting string to the message buffer
	strcpy(messageBuffer, tempStartingPoint);

	int totalCharacterAlreadyAdded = hexLength + lengthOfNumOfTimes + 1;	// Find out how many characters have already been added
	for (index = totalCharacterAlreadyAdded; index < bufferSize; index++)	//Message buffer size can increase from 1,000 - 10,000 bytes
	{
		if (elementValue < 58)
		{
			messageBuffer[index] = (char)elementValue;	//Fill the message buffer with the elements value from 0 - 9
			elementValue++;
		}

		if (elementValue == 58)
		{
			elementValue = 48;							//Reset the value once it passes decimal 58 (ie. ascii '9' = (char)58)
		}
	}

	messageBuffer[index] = '\0';

}//Done


#if defined __linux__
/*
*  FUNCTION      : stopWatch
*  DESCRIPTION   : This function is used to get the number of milliseconds since the Epoch  (Jan 1, 1970)
*  PARAMETERS    : void: The function takes no arguments
*  RETURNS       : long : Returns the current microsecond count
*
*	NOTE: This function  was initially found online. Since then, the function has been partial modified to suit the projects needs.
*		   As a result, credit belongs to the original author on the website. For more information, please see the reference,
*		   Lee. (2018). How to measure time in milliseconds using ANSI C?. Retrieved on January 8, 2019,
*			from https://stackoverflow.com/questions/361363/how-to-measure-time-in-milliseconds-using-ansi-c/36095407#36095407
*/
double stopWatch(void)
{
	//struct contains the following fields:
	/*
		struct timeval {
			time_t      tv_sec;     //seconds
			suseconds_t tv_usec;    //microseconds
		};
	*/

	struct timeval time;
	if (gettimeofday(&time, NULL) == 0)					//Return of 0 indicates success
	{
		return (time.tv_usec  / 1000);					//Milliseconds = (microseconds  / 1000)
	}
	return ERROR_RETURN;

}//Done


/*
*  FUNCTION      : calculateElapsedTime
*  DESCRIPTION   : This function is used to calculate the elapsed time for message transmission between the networked clients and server
*  PARAMETERS    : long startTime : Start time for when the transmission began
*				   long endTime	  : End time for when the transmission had finished
*  RETURNS       : double : Returns the elapsedTime time between the two values
*/
double calculateElapsedTime(long startTime, long endTime)
{
	return (endTime - startTime);

}//Done
#endif


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