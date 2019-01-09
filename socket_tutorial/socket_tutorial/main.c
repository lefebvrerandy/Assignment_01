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
#include "client.h"
#include "server.h"
#include "shared.h"
#pragma warning(disable: 4996)


/*
*  FUNCTION      : proc_arguments
*  DESCRIPTION   : DEBUG
*  PARAMETERS    : Parameters are as follows, 
*	int argumentCount: DEBUG
*	char* args[]	 : DEBUG
*  RETURNS       : int : DEBUG
*/
int proc_arguments(int argumentCount, char* args[])
{
	char expectedSwitch[4][15] = { {"-a"}, {"-p"}, {"-s"}, {"-argumentCount"} };


    // only two argument besides the actual command allowed
    if(argumentCount == 3) 
	{
		if (strcmp(args[1], "-p") == 0)
		{
			strcpy(storedData[2], args[2]);
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

		// Check the Type of Connection
		if (strcmp(args[1], "-TCP") == 0)
			strcpy(storedData[0], args[1]);
		else if (strcmp(args[1], "-UDP") == 0)
			strcpy(storedData[0], args[1]);

		// Iterate through the arguments starting at 2 and iterating by 2 each time through the loop
		int j = 0;
		for (int i = 2; i < 9; i++)
		{

			// If the expected is found, store the data into the 2d array called "StoredData" 
			if (strcmp(args[i], expectedSwitch[j]) == 0)
			{
				strcpy(storedData[j], args[i + 1]);

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

	if (strcmp(storedData[0], "-TCP") == 0)
	{
		return 2;
	}
	else if (strcmp(storedData[0], "-UDP") == 0)
	{
		return 3;
	}
    return 0;
}


/*
*  FUNCTION      : validateAddress
*  DESCRIPTION   : DEBUG
*  PARAMETERS    : DEBUG
*  RETURNS       : DEBUG
*/
int validateAddress(char string[])
{
	int res = 0;
	// Check the address string to make sure its an address.
	// If there is an error, return 1;


	return res;
}


/*
*  FUNCTION      : validatePort
*  DESCRIPTION   : DEBUG
*  PARAMETERS    : DEBUG
*  RETURNS       : DEBUG
*/
int validatePort(char string[])
{
	int res = 0;
	// Check the port string to make sure its a port.
	// If there is an error, return 1;

	return res;
}


/*
*  FUNCTION      : validateBlockSize
*  DESCRIPTION   : DEBUG
*  PARAMETERS    : DEBUG
*  RETURNS       : DEBUG
*/
int validateBlockSize(char string[])
{
	int res = 0;
	// Check the block size string to make sure its a valid size.
	// If there is an error, return 1;

	return res;
}


/*
*  FUNCTION      : validateNumOfBlocks
*  DESCRIPTION   : DEBUG
*  PARAMETERS    : DEBUG
*  RETURNS       : DEBUG
*/
int validateNumOfBlocks(char string[])
{
	int res = 0;
	// Check the number of blocks string to make sure its a valid size.
	// If there is an error, return 1;

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
        start_client_protocol(IPPROTO_TCP);
        break;
	case 3:
		start_client_protocol(IPPROTO_UDP);
		break;
    }

    // cleanup WinSock in Windows
#if defined _WIN32
    WSACleanup();
#endif


    return 0;
}