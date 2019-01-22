/*
*  FILE          : client.c
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains the functions and logic required to execute the client's required functionality.
*				   Functions are included for creating, connecting, and closing sockets, and for sending/receiving messages
*				   to and from the server
*/

#if defined _WIN32
#include "client.h"
#elif defined __linux__
#include "../inc/client.h"
#endif



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
	struct sockaddr_in socketAddress;								//Local address struct
	memset((void*)&socketAddress, 0, sizeof(socketAddress));		//Clear the socket struct before initialization


	//Stage 1: Setup the client's address struct
	socketAddress.sin_family = AF_INET;											
	socketAddress.sin_addr.s_addr = inet_addr(storedData[CLA_IP_ADDRESS]);
	socketAddress.sin_port = htons((u_short)(storedData[CLA_PORT_NUMBER]));


	//Stage 2: Host data has been retried and set, proceed to open the socket
	SOCKET openSocketHandle = createSocket(AF_INET, stream_or_datagram, tcp_or_udp);
	if (!(openSocketHandle > ERROR_RETURN))
	{
		printError(SOCKET_CREATION_ERROR);
		return  SOCKET_CREATION_ERROR; 
	}


	//Stage 3: Connect to the server
	int boundSocketHandle = connectToServer(openSocketHandle, socketAddress);
	if (!(boundSocketHandle > SOCKET_ERROR))
	{
		printError(SOCKET_CONNECTION_ERROR);
		return SOCKET_CONNECTION_ERROR;
	}


	//Stage 4: Convert the CLA's and allocate space for the message buffer
	int numberOfBlocks = convertCharToInt(storedData[CLA_NUMBER_OF_BLOCKS]);
	int blockSize = convertCharToInt(storedData[CLA_BUFFER_SIZE]);
	int totalBytes = blockSize * numberOfBlocks;
	char* messageBuffer;


	//Stage 5: Start the message loop
	Timer stopwatch;
	#if defined _WIN32
	stopwatch.startTime = GetTickCount();				//Start the Windows timer

	#elif defined __linux__
	stopwatch.startTime = stopWatch();							//Start the UNIX timer

	#endif
	int currentblockCount = 0;
	do		
	{
		messageBuffer = CreateMessageBuffer(blockSize, numberOfBlocks, currentblockCount + 1);
		sendMessage(openSocketHandle, messageBuffer);	//Send the blocks across the network
		currentblockCount++;
	} while (currentblockCount < numberOfBlocks);

	#if defined _WIN32
	stopwatch.endTime = GetTickCount();					//Stop the Windows timer

	#elif defined __linux__
	stopwatch.endTime = stopWatch();								//Stop the UNIX timer

	#endif
	stopwatch.elapsedTime = stopwatch.endTime - stopwatch.startTime;


	//Stage 7: Receive the missing blocks results from the server
	memset((void*)messageBuffer, 0, sizeof(messageBuffer));
	recv(openSocketHandle, messageBuffer, sizeof(messageBuffer), 0);
	int proportionMissing = convertCharToInt(messageBuffer);


	//Stage 8: Receive the disorganized results from server
	memset((void*)messageBuffer, 0, sizeof(messageBuffer));
	recv(openSocketHandle, messageBuffer, sizeof(messageBuffer), 0);
	int disordered = convertCharToInt(messageBuffer);


	//Stage 9: Analyze the results, and print them to the screen 
	int megaBitsPerSecond = calculateSpeed(totalBytes, (int)stopwatch.elapsedTime);
	printResults(blockSize, numberOfBlocks, (int)stopwatch.elapsedTime, megaBitsPerSecond, proportionMissing, disordered);	//Size: <<size>> Sent: <<sent>> Time: <<time>> Speed: <<speed>> Missing: <<missing>> Disordered: <<disordered>> 
	free(messageBuffer);


	//Close the sockets
	#if defined _WIN32
		closesocket(openSocketHandle);
	#elif defined __linux__
		close(openSocketHandle);
	#endif
	return SUCCESS;
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
}


/*
*  FUNCTION      : CreateMessageBuffer
*  DESCRIPTION   : This function is used to reserve a chunk of space in memory with the size 
*				   defined at run time by the CLA's
*  PARAMETERS    : Parameters are as follows,
*	int bufferSize		: Size of memory to reserve for the message buffer
*	int numberOfBlocks	: Number of blocks that will be converted to hex
*	int currentMsgNum	: Current message ID that will be converted to hex
*  RETURNS       : char* : The function has no return value
*/
char* CreateMessageBuffer(int bufferSize, int numberOfBlocks, int currentMsgNum)
{
	
	char* returnArray = malloc(sizeof(char) * (bufferSize + 1));
	char messageProperties[MESSAGE_PROPERTY_SIZE] = { "" };


	//Set the message buffer's properties
	setMessageProperties(messageProperties, bufferSize, numberOfBlocks, currentMsgNum);
	strcpy(returnArray, messageProperties);
	

	//Find the amount of space occupied by the message properties, and offset the index
	int propertyLength = strlen(returnArray);	


	//Fill each block with chars 0 - 9
	fillMessageBuffer(returnArray, bufferSize, propertyLength);					
	return returnArray;
}


/*
*  FUNCTION      : fillMessageBuffer
*  DESCRIPTION   : This function is used to set the message properties (block size and count) and 
*				   print them at the start of the outbound message array for the server
*  PARAMETERS    : Parameters are as follows,
*	int messageProperties[] : Array that will hold the message properties (ie. block size and count)
*	int bufferSize			: The block size supplied by the CLA
*	int numberOfBlocks		: The number of blocks supplied by the CLA
*	int currentMsgNum		: The current message number used for filling in the block ID
*  RETURNS       : void : The function has no return value
*/
void setMessageProperties(char messageProperties[], int bufferSize, int numberOfBlocks, int currentMsgNum)
{

	//////////////////////////////////////////////////////////////////////////////////
	// 
	// The start to each of the messages will contain the block size (-s) and count (-n)
	//		The first 4 characters are a hexadecimal converting number that contains
	//		the block size that will be sent.
	//		The fifth character, and until the first "G" will be the amount of messages
	//		that will be passed.
	//
	//		XXXX = Hex code of how many bytes are coming
	//		YYYY = Hex code of how many times its sending
	//		XXXX = Which message number we are sending
	//		G(or some other character thats not a - f | 0 - 9) = ending the information.
	/////////////////////////////////////////////////////////////////////////////////

	// Convert how many bytes are coming
	switch (bufferSize)											
	{
		case MESSAGE_BUFFER_SIZE_1000:	
			strcpy(messageProperties, "03E8");	//Hex to decimal 1000 bytes
			break;

		case MESSAGE_BUFFER_SIZE_2000:
			strcpy(messageProperties, "07D0");	//2000 bytes
			break;

		case MESSAGE_BUFFER_SIZE_5000:
			strcpy(messageProperties, "1388");	//5000 bytes
			break;

		case MESSAGE_BUFFER_SIZE_10000:
			strcpy(messageProperties, "2710");	//10,000 bytes
			break;
	}


	// Convert how many times its sending
	// Convert the decimal number of Number of Blocks to hex and store it
	char numBlocks[MESSAGE_PROPERTY_SIZE] = { "" };
	convertDecToHex(numberOfBlocks, numBlocks);
	strcat(messageProperties, numBlocks);


	// Convert which message number we are sending
	// Convert the decimal number of which iteration is being sent to hex and store it
	char msgNumHex[5] = {""};
	convertDecToHex(currentMsgNum, msgNumHex);
	strcat(messageProperties, msgNumHex);


	// Append the ending "G" at the end of the format
	strcat(messageProperties, "G");
}


/*
*  FUNCTION      : fillMessageBuffer
*  DESCRIPTION   : This function is used to fill the messageBuffer with numbers 0 - 9
*  PARAMETERS    : Parameters are as follows,
*	int messageBuffer[] : Message buffer which will be filled with integers
*	int bufferSize		: The block size supplied by the CLA
*	int messageIndexOffset : The amount of space already occupied by the message properties 
*							 at the start of the outbound message array
*  RETURNS       : void : The function has no return value
*/
void fillMessageBuffer(char messageBuffer[], int bufferSize, int messageIndexOffset)
{
	int index = 0;
	int elementValue = ASCII_VALUE_0;									//ASCII 48 is '0'
	for (index = messageIndexOffset; index < bufferSize; index++)		//Message buffer size can range from 1,000 - 10,000 bytes
	{
		if (elementValue < ASCII_VALUE_9)								//ASCII 58 is the char after '9'
		{
			messageBuffer[index] = (char)elementValue;					//Fill the message buffer with the elements value from 0 - 9
			elementValue++;
		}

		if (elementValue == ASCII_VALUE_9)
		{
			elementValue = ASCII_VALUE_0;								//Reset the value once it passes decimal 58 (ie. ascii '9' = (char)58)
		}
	}


	//Terminate the string with the carriage return
	messageBuffer[index] = '\0';

}


#if defined __linux__
/*
*  FUNCTION      : stopWatch
*  DESCRIPTION   : This function is used to get the number of milliseconds since the Epoch  (Jan 1, 1970)
*  PARAMETERS    : void: The function takes no arguments
*  RETURNS       : long : Returns the current microsecond count
*
*	NOTE: This function  was initially found online. Since then, the function has been partial modified to suit the projects needs.
*		   As a result, credit belongs to the original author on the website. For more information, please see the reference,
*		   Lee. (2018). How to measure time in milliseconds using ANSI C?. Retrieved on January 8, 2019, from 
				https://stackoverflow.com/questions/361363/how-to-measure-time-in-milliseconds-using-ansi-c/36095407#36095407
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

}


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

}
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

	int megabytes = bytes / MEGABYTES;			//Constant set to 10242 bytes per megabyte
	int seconds = elapsedTimeMS / MILLISECONDS;	//Constants set to 1000 milliseconds per second


	//Speed is calculated as (megabytes / sec) * 8 (ie. megabits)
	int speed = ((megabytes) / elapsedTimeMS) * 8;
	return speed;
}


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
	printf("Size: %d Sent: %d, Time: %dms, Speed: %dMbps , Missing: %d, Disordered: %d", size, sent, time, speed, missing, disordered);
}


/*
*  FUNCTION      : convertDecToHex
*  DESCRIPTION   : In this function, we will take a decimal number and convert it to a hex number
*					in the format of XXXX. It must be 4 characters long in order to work with our protocol.
*  PARAMETERS    : parameters are as follows,
*	int decimal	  : The decimal number to convert
*	char* hexaNum : The Hex char array that will contain the 4 character long hex number
*  RETURNS       : void : The function has no return value
*/
void convertDecToHex(int decimal, char* hexaNum)
{
	long decimalNum;
	long quotient;
	long remainder;
	int i = 0;
	int j = 0;
	char hexaNumBackwards[MAX_FORMAT_SIZE + 1] = { "" };


	//The conversion of turning our decimal number into a hex number
	quotient = (long)decimal;
	while (quotient != 0)
	{
		remainder = quotient % 16;
		if (remainder < 10)
			hexaNumBackwards[j++] = ASCII_VALUE_0 + remainder;
		else
			hexaNumBackwards[j++] = (char)(55 + remainder);
		quotient = quotient / 16;
	}

	//Reverse the order of the hex characters calculated above
	int l = 0;
	for (i = j; i > 0; i--)
	{
		hexaNum[l] = hexaNumBackwards[i - 1];
		l++;
	}


	// Add any 0's at the beginning of the string to create the format of
	// XXXX. We need to make sure the number returned is 4 numbers long to 
	// follow our protocol
	char buffer[MAX_FORMAT_SIZE + 1] = { "" };
	if (strlen(hexaNumBackwards) < MAX_FORMAT_SIZE)
	{

		int strLength = 0;
		strLength = strlen(hexaNumBackwards);
		for (int i = strLength; i < MAX_FORMAT_SIZE; i++)
		{
			strcat(buffer, "0");
		}
		strcat(buffer, hexaNum);
		strcpy(hexaNum, buffer);
	}
}