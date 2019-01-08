/*
*  FILE          : main.c
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains main, and acts as the primary controller for the solution. Functions are included for 
*
*  NOTE: DEBUG ADD THE REFERENCE TO NORBERTS PREVIOUS WORK, AND THE ONLINE POST HE GOT IT FROM
*/


//OS Independent Headers
#include <string.h>
#include <stdio.h>
#include <time.h>


//OS Dependent Headers
#if defined _WIN32
#include <winsock.h>  // WinSock subsystem
#elif defined __linux__
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif


// redefine some types and constants based on OS
#if defined _WIN32
typedef int socklen_t;  // Unix socket length
#elif defined __linux__
typedef int SOCKET;
#define INVALID_SOCKET -1  // WinSock invalid socket
#define SOCKET_ERROR   -1  // basic WinSock error
#define closesocket(s) close(s);  // Unix uses file descriptors, WinSock doesn't...
#endif


/*
================
  server function
================
*/
int start_server_TCP()
{
	struct sockaddr_in socketAddress;		// address variables
	struct sockaddr_in remoteAddress;		// address variables
	SOCKET openSocketHandle;				//Sockets
	SOCKET acceptedSocketConnection;		//Sockets
	int    boundSocketHandle;				//Holds return values?
	socklen_t addressLength = sizeof(remoteAddress);	// the length of our remote address


    // create the local socket
	//Protocol/Address family, type of socket (socket stream ie. tcpip/datagram stream ie. UDP), end to end protocol
	//PF_INET, SOCK_DGRAM, IPPROTO_UDP
    openSocketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(INVALID_SOCKET == openSocketHandle) {
        perror("Could not create socket");
        return -1;
    }

    // setup the local address variable
    memset((void*)&socketAddress, 0, sizeof(socketAddress));
    socketAddress.sin_family      = AF_INET;
    socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    socketAddress.sin_port        = htons(6868);

    // name the local socket
    boundSocketHandle = bind(openSocketHandle, (struct sockaddr*)&socketAddress, sizeof(socketAddress));
    if(SOCKET_ERROR == boundSocketHandle) {
        perror("Could not bind to local socket");
        closesocket(openSocketHandle);
        return -1;
    }

    // set the socket to listen for a connection
    boundSocketHandle = listen(openSocketHandle, SOMAXCONN);
    if(SOCKET_ERROR == boundSocketHandle) {
        perror("Could not listen to local socket");
        closesocket(openSocketHandle);
        return -1;
    }

    // a little user interaction... ;)
    printf("Waiting for connection... ");
    fflush(stdout);

    // wait for a connection
    acceptedSocketConnection = accept(openSocketHandle, (struct sockaddr*)&remoteAddress, &addressLength);
    if(INVALID_SOCKET == acceptedSocketConnection) {
        perror("Could not accept new connection");
        closesocket(openSocketHandle);
        return -1;
    }
    printf("accepted.\n");

    // send some data to the newly connected client
    char data[] = "Hello client!\0";
    char recieved[256];
    memset((void*)recieved, 0, sizeof(recieved));
    send(acceptedSocketConnection, data, strlen(data), 0);

    // wait for a reply
    recv(acceptedSocketConnection, recieved, sizeof(recieved), 0);
    printf("Client returned: %s\n", recieved);

    // cleanup is VERY, VERY important with sockets!!!
    // they are file descriptors (and you only get so many in the life of an OS)
    closesocket(acceptedSocketConnection);
    closesocket(openSocketHandle);

    return 0;
}


/*
================
  client function
================
*/
int start_client_TCP()
{
    struct sockaddr_in addr;  // local address variable
    SOCKET   s;               // local socket
    int      r;               // will hold return values
    hostent* h;               // server host entry (holds IPs, etc)
    const char local_host[] = "localhost";

	//clock_t startTime = stopWatch();
	/*
		Do all the work here
	*/
	//clock_t endTime = stopWatch();
	//double elapsedTime = calculateElapsedTime(startTime, endTime);

    // get the server host entry
    memset((void*)&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(local_host);
    if(INADDR_NONE == addr.sin_addr.s_addr) {
        h = gethostbyname(local_host);
        if(NULL == h) {
            perror("Could not get host by name");
            return -1;
        }
    } else {
        h = gethostbyaddr((const char*)&addr.sin_addr, sizeof(struct sockaddr_in), AF_INET);
        if(NULL == h) {
            perror("Could not get host by address");
            return -1;
        }
    }

    // create the local socket
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(INVALID_SOCKET == s) {
        perror("Could not create socket");
        return -1;
    }

    // setup the rest of our local address
    addr.sin_family = AF_INET;
    addr.sin_addr   = *((in_addr*)*h->h_addr_list);
    addr.sin_port   = htons(6868);

    // a little user interaction... ;)
    printf("Connecting... ");
    fflush(stdout);

    // connect to the server
    r = connect(s, (sockaddr*)&addr, sizeof(struct sockaddr));
    if(SOCKET_ERROR == r) {
        perror("Cannot connect to server");
        closesocket(s);
        return -1;
    }
    printf("connected.\n");

    // recieve the servers packet and reply
    char data[] = "Hello server!\0";
    char recieved[256];
    memset((void*)recieved, 0, sizeof(recieved));
    recv(s, recieved, sizeof(recieved), 0);
    printf("Server sent: %s\n", recieved);
    send(s, data, strlen(data), 0);

    // cleanup, cleanup, cleanup!!!
    closesocket(s);

    return 0;
}


/*
================
  process command line arguments

  gets passed argc and argv from main()
================
*/
int proc_arguments(int n, char* args[])
{
	char tempArgument[50] = { "" };
	// CONVERT THIS TO A 2d ARRAY for easy access
	char port[10] = {""};					//< Port of the server
	char typeOfConnection[5] = { "" };		//< TCP or UDP
	char ipAddressOfServer[15] = { "" };	//< Ip address of the server
	char blockSize[3] = { "" };				//< Block size to send
	char numOfBlocks[3] = { "" };			//< Number of blocks to send
	// 2d ARRAY ATTEMPT
	char storedData[5][15];
	char expectedSwitch[4][15] = { {"-a"}, {"-p"}, {"-s"}, {"-n"} };


    // only two argument besides the actual command allowed
    if(n == 3) 
	{
		if (strcmp(args[1], "-p") == 0)
		{
			strcpy(port, args[2]);
		}
    }

	// If 10 arguments, must mean to start client.
	else if (n == 10)
	{


		/*
			This section checks and stores the proper arguments into place for later use
			REFERENCE: INDEX LOCATION
				0			1	 2		3	 4	 5		 6		   7	 8			9
		./ispeed	-TCP/-UDP	-a ADDRESS	-p PORT		-s BLOCK_SIZE	-n NUM_BLOCKS
		*/

		// Check the Type of Connection
		if (strcmp(args[1], "-TCP") == 0)
			strcpy(tempArgument, args[1]);
		else if (strcmp(args[1], "-UDP") == 0)
			strcpy(tempArgument, args[1]);

		// Check to see if tempArgument is empty, if so return an eror. If not, store into char array
		if (strcmp((const char*)tempArgument[0], '\0') == 0) // THIS NEEDS TESTING
			return -1;
		else
		{
			strcpy(storedData[0], tempArgument);
		}

		// Iterate through the arguments starting at 2 and iterating by 2 each time through the loop
		int j = 1;
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

    return 0;
}

int validateAddress(char string[])
{
	int res = 0;
	// Check the address string to make sure its an address.
	// If there is an error, return 1;


	return res;
}

int validatePort(char string[])
{
	int res = 0;
	// Check the port string to make sure its a port.
	// If there is an error, return 1;

	return res;
}

int validateBlockSize(char string[])
{
	int res = 0;
	// Check the block size string to make sure its a valid size.
	// If there is an error, return 1;

	return res;
}

int validateNumOfBlocks(char string[])
{
	int res = 0;
	// Check the number of blocks string to make sure its a valid size.
	// If there is an error, return 1;

	return res;
}

/*
================
  program starts here

  gets the number of arguments on the command line,
    and the actual arguments
================
*/
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
    case 0:  // invalid options
        printf("Usage: socket_tutorial [option]\n");
        printf("Options:\n");
        printf("-c - start a client\n");
        printf("-s - start a server\n\n");
        break;
    case 1:
        start_server_TCP();  // start a server
        break;
    case 2:
        start_client_TCP();  // start a client
        break;
    }

    // cleanup WinSock in Windows
#if defined _WIN32
    WSACleanup();
#endif


    return 0;
}


/*
*  FUNCTION      : stopWatch
*  DESCRIPTION   : 
*  PARAMETERS    : 
*  RETURNS       : 
*
*	NOTE: This function  was initially found online, however, the original soruce code has since been modified to suit the projects needs. 
		   As a result, partial credit belongs to the original authors on the website. For more information, please see the reference,
		   GeeksForGeeks.(ND). How to measure time taken by a function in C?. Retrieved on January 8, 2019, 
			from https://www.geeksforgeeks.org/how-to-measure-time-taken-by-a-program-in-c/
*/
clock_t stopWatch()
{
	clock_t clockTime = clock();
	return clockTime;
}


/*
*  FUNCTION      : calculateElapsedTime
*  DESCRIPTION   :
*  PARAMETERS    :
*  RETURNS       :
*/
double calculateElapsedTime(clock_t startTime, clock_t endTime)
{
	double cpu_time_used = ((double)(endTime - startTime)) / CLOCKS_PER_SEC;
	return cpu_time_used;
}