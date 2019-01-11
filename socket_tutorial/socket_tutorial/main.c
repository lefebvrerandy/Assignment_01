/*
*  FILE          : main.c
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains main, and acts as the primary controller for the solution. Functions are included for DEBUG
*
*  NOTE: DEBUG ADD THE REFERENCE TO NORBERTS PREVIOUS WORK, AND THE ONLINE POST HE GOT IT FROM
*/


//Prototypes OS specific
#if defined _WIN32

#elif defined __linux__

#endif


//Standard C headers
#include "shared.h"



/*
*  FUNCTION      : proc_arguments
*  DESCRIPTION   : DEBUG
*  PARAMETERS    : Parameters are as follows, 
*	int argumentCount: DEBUG
*	char* args[]	 : DEBUG
*  RETURNS       : int : Denotes if the operation completed successfully (ie. return > -1)
*/
int proc_arguments(int argumentCount, char* args[])
{
	char expectedSwitch[SWITCH_OPTIONS][MAX_ARGUMENT_LENGTH] = { {"-a"}, {"-p"}, {"-s"}, {"-n"} };


    // Only two argument besides the actual command allowed
    if(argumentCount == 3) 
	{
		if (strcmp(args[1], "-p") == 0)
		{
			strcpy(storedData[CLA_PORT_NUMBER], args[CLA_PORT_NUMBER]);		//CLA_PORT_NUMBER is set as 2 in accordance with storedData's declaration in shared.h
		}
		return 1;
    }


	// If 10 arguments, must mean to start client.
	else if (argumentCount == 10)
	{


		/*
			This section checks and stores the proper arguments into place for later use
			REFERENCE: INDEX LOCATION
				0			1	 2		3	 4	 5		 6		   7	 8			9
		./ispeed	-TCP/-UDP	-a ADDRESS	-p PORT		-s BLOCK_SIZE	-n NUM_BLOCKS
		*/

		/// Print all arguments to screen
		//for (int i = 0; i < 10; i++)
		//{
		//	printf("%d = %s\n",(i+1), args[i]);
		//}

		// Check the Type of Connection
		if (strcmp(args[1], "-TCP") == 0)
			strcpy(storedData[0], args[1]);
		else if (strcmp(args[1], "-UDP") == 0)
			strcpy(storedData[0], args[1]);

		// Iterate through the arguments starting at 2 and iterating by 2 each time through the loop
		int j = 0;
		for (int i = 2; i < 10; i++)
		{
			// If the expected is found, store the data into the 2d array called "StoredData" 
			if (strcmp(args[i], expectedSwitch[j]) == 0)
			{
				strcpy(storedData[j+1], args[i + 1]);

				int res = 0;
				switch (j)
				{
				case 0:
					res = validateAddress(storedData[j]);
					break;
				case 1:
					res = validatePort(storedData[j]);
					break;
				case 2:
					res = validateBlockSize(storedData[j]);
					break;
				case 3:
					res = validateNumOfBlocks(storedData[j]);
					break;
				}


				if (res == 1)
				{
					return -3;
				}
			}
			else
			{
				return -2;	// Return an error that a switch was misplaced or not found
			}

			i++; // Index for argument
			j++; // Index for the 2d storedData array
		}
	}

	// Not the correct amount of arguments.. Display the instructions (Might not be smart to display instructions
	// because it will conflict with a batch file)..
	else
	{
		
	}



	if (strcmp(storedData[CLA_SOCKET_TYPE], "-TCP") == 0)			//CLA_SOCKET_TYPE is set to 0, in accordance with storedData's declaration in shared.h
	{
		return 2;	//DEBUG NEED TO CHANGE FROM MAGIC NUMBER
	}
	else if (strcmp(storedData[CLA_SOCKET_TYPE], "-UDP") == 0)
	{
		return 3;	//DEBUG NEED TO CHANGE FROM MAGIC NUMBER
	}
    return 0;
}


/*
*  FUNCTION      : validateAddress
*  DESCRIPTION   : This function is used to check if the IP address supplied from the command line, is valid according to the
*					standards set by IPv4 (ie. its a 32-bit number of form DDD.DDD.DDD.DDD)
*  PARAMETERS    : char address[] : String containing the IP address
*  RETURNS       : int : Denotes if the operation completed successfully (ie. return > -1)
*/
int validateAddress(char address[])
{
	int addressValid = -1;


	//Check if the address in the form of IPv4.
	int errorCount = 0;
	int IPaddressLength = strlen(address);
	if (IPaddressLength == 32)											//IPv4 is 32 bits in length DDD.DDD.DDD.DDD (ex. 192.168.2.100)
	{
		int index = 0;
		for (index = 0; index < IPaddressLength; index++)	
		{
			if (index == (3 || 7 || 11))								//DEBUG might need to rewrite the statement to check for each value individually
			{
				if (address[index] != '.')
				{
					errorCount++;
				}
			}
			else
			{
				if (!((address[index] >= '0') && (address[index] <= '9')))	//Check if the character is a digit of 0 - 9
				{
					errorCount++;
				}
			}
		}

		if (errorCount > 0)
		{
			addressValid = -1;		//Errors detected, address was not valid
		}

		else
		{
			addressValid = 1;		//No errors, address is valid
		}
	}



	return addressValid;

}//Done 


/*
*  FUNCTION      : validatePort
*  DESCRIPTION   : DEBUG
*  PARAMETERS    : DEBUG
*  RETURNS       : int : Denotes if the operation completed successfully (ie. return > -1)
*/
int validatePort(char string[])
{
	int res = 0;
	// Check the port string to make sure its a port.
	// If there is an error, return -1;

	return res;
}


/*
*  FUNCTION      : validateBlockSize
*  DESCRIPTION   : This function is used to check if the block size is valid
*  PARAMETERS    : char blockSizeString[] : String containing the block size
*  RETURNS       : int : Denotes if the operation completed successfully (ie. return > -1)
*/
int validateBlockSize(char blockSizeString[])
{
	int blockSizeValid = 0;
	int blockSize = convertCharToInt(blockSizeString);
	switch (blockSize)
	{
		case MESSAGE_BUFFER_SIZE_1000:
		case MESSAGE_BUFFER_SIZE_2000:
		case MESSAGE_BUFFER_SIZE_5000:
		case MESSAGE_BUFFER_SIZE_10000:
			blockSizeValid = 1;				//Valid block size
			break;

		default:
			blockSizeValid = -1;			//Invalid block size
			break;
	}

	return blockSizeValid;

}//Done


/*
*  FUNCTION      : validateNumOfBlocks
*  DESCRIPTION   : DEBUG
*  PARAMETERS    : DEBUG
*  RETURNS       : int : Denotes if the operation completed successfully (ie. return > -1)
*/
int validateNumOfBlocks(char string[])
{
	int res = 0;
	// Check the number of blocks string to make sure its a valid size.
	// If there is an error, return -1;

	return res;
}

int main(int argc, char* argv[])
{
    // startup WinSock in Windows
#if defined _WIN32
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(1,1), &wsa_data);
#endif

    // process the command line arguments
	// If 1 argument, must be start server. 
	// If 5 arguments, must be start client.
    switch(proc_arguments(argc, argv))
    {
    case 1:
        start_server();
        break;
    case 2:
        start_client_protocol(SOCK_STREAM, IPPROTO_TCP);
        break;
	case 3:
		start_client_protocol(SOCK_DGRAM, IPPROTO_UDP);
		break;
    }

    // cleanup WinSock in Windows
#if defined _WIN32
    WSACleanup();
#endif


    return 0;
}