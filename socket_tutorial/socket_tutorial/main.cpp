/*
*  FILE          : main.c
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains main, and acts as the primary controller for the solution. Functions are included for 
*
*  NOTE: DEBUG ADD THE REFERENCE TO NORBERTS PREVIOUS WORK, AND THE ONLINE POST HE GOT IT FROM
*/


//Prototypes
int validateAddress(char string[]);
int validatePort(char string[]);
int validateBlockSize(char string[]);
int validateNumOfBlocks(char string[]);
int start_server_protocol(int tcp_or_udp);
int start_client_protocol(int tcp_or_udp);
SOCKET createSocket(void);
clock_t stopWatch(void);
double calculateElapsedTime(clock_t startTime, clock_t endTime);

//Prototypes OS specific
#if defined _WIN32

#elif defined __linux__

#endif

// Global struct for all client connection info
char storedData[5][15];
// storedData Breakdown
//	[0][] = TCP || UDP
//	[1][] = IP Address
//	[2][] = Port
//	[3][] = Size of buffer to send
//	[4][] = Number of blocks to send


// standard C headers
#include <string.h>
#include <stdio.h>
#include <time.h>

#pragma warning(disable: 4996)

//OS Dependent Headers
#if defined _WIN32
#include <winsock.h>  // WinSock subsystem
#include <windows.h>
#elif defined __linux__
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
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

#define PORT 6868
#define MESSAGE_SIZE 256

/*
================
  server functions
================
*/
int start_server()
{
	// Spawn two threads. One for TCP, one for UDP
#if defined _WIN32
	HANDLE thread_windows_server[2];
	thread_windows_server[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)start_server_protocol, (LPVOID)IPPROTO_TCP, 0, NULL);
	thread_windows_server[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)start_server_protocol, (LPVOID)IPPROTO_UDP, 0, NULL);

	WaitForMultipleObjects(2, thread_windows_server, TRUE, INFINITE);

	for (int i = 0; i < 2; i++)
	{
		CloseHandle(thread_windows_server[i]);
	}

#elif defined __linux__
	pthread_t thread_linux_server[2];
	if (pthread_create(&thread_linux_server[0], NULL, start_server_protocol, (void*)IPPROTO_TCP) != 0)
	{
		// An error has occured
		perror("Could not create Thread.");
	}
	else if (pthread_create(&thread_linux_server[1], NULL, start_server_protocol, (void*)IPPROTO_UDP) != 0)
	{
		// An error has occured
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

	// TEMP - default to TCP
	//start_server_TCP();

	return 0;
}

/*
================
  server TCP
================
*/
int start_server_protocol(int tcp_or_udp)
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
    openSocketHandle = socket(AF_INET, SOCK_STREAM, tcp_or_udp);
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
int start_client_protocol(int tcp_or_udp)
{
    struct sockaddr_in socketAddress;	//local address variable
    int      boundSocketHandle;			//Holds transmission results
    hostent* h;							//server host entry (holds IPs, etc)
    const char local_host[] = "localhost";

	//clock_t startTime = stopWatch();
	/*
		Do all the work here
	*/
	//clock_t endTime = stopWatch();
	//double elapsedTime = calculateElapsedTime(startTime, endTime);


    //Clear the socket struct before initializing it
    memset((void*)&socketAddress, 0, sizeof(socketAddress));
    socketAddress.sin_addr.s_addr = inet_addr(local_host);
    if(INADDR_NONE == socketAddress.sin_addr.s_addr)
	{
        h = gethostbyname(local_host);
        if(NULL == h) 
		{
            perror("Could not get host by name");
            return -1;
        }
    }
	else 
	{
        h = gethostbyaddr((const char*)&socketAddress.sin_addr, sizeof(struct sockaddr_in), AF_INET);
        if(NULL == h) {
            perror("Could not get host by address");
            return -1;
        }
    }

   
	SOCKET openSocketHandle = createSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    openSocketHandle = socket(AF_INET, SOCK_STREAM, tcp_or_udp);

	SOCKET openSocketHandle = createSocket();
	socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(openSocketHandle == INVALID_SOCKET)
	{
		perror("[ERROR]: Could not open the client socket");
		return -1;
	}

    // setup the rest of our local address
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr   = *((in_addr*)*h->h_addr_list);
    socketAddress.sin_port   = htons(PORT);

    // a little user interaction... ;)
    printf("Connecting... ");
    fflush(stdout);

    // connect to the server
    boundSocketHandle = connect(openSocketHandle, (sockaddr*)&socketAddress, sizeof(struct sockaddr));
    if(SOCKET_ERROR == boundSocketHandle)
	{
        perror("Cannot connect to server");
        closesocket(openSocketHandle);
        return -1;
    }
    printf("connected\n");	//DEBUG remove before submission


    //recieve the servers packet and reply
    char message[] = "outBoundMessage\0";
    char recieved[MESSAGE_SIZE] = "";


    memset((void*)recieved, 0, sizeof(recieved));		//Clear the 
    recv(openSocketHandle, recieved, sizeof(recieved), 0);
    printf("Server sent: %s\n", recieved);
    send(openSocketHandle, message, strlen(message), 0);


    closesocket(openSocketHandle);
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
	char expectedSwitch[4][15] = { {"-a"}, {"-p"}, {"-s"}, {"-n"} };


    // only two argument besides the actual command allowed
    if(n == 3) 
	{
		if (strcmp(args[1], "-p") == 0)
		{
			strcpy(storedData[2], args[2]);
		}
		return 1;
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


/*
*  FUNCTION      : createSocket
*  DESCRIPTION   : This function is used to create and initialze a socket with the appropriate properties as set by the parameterd
*  PARAMETERS    : DEBUG
*  RETURNS       : SOCKET : Returns an initialized socket
*/
SOCKET createSocket(int protocolDomain, int socketType, int protocolType)
{
	SOCKET newSocket = socket(protocolDomain, socketType, protocolType);
	return newSocket;

}//Done


/*
*  FUNCTION      : stopWatch
*  DESCRIPTION   : This function is used to get the number of clock ticks since the process started. It returns the tick count
				   and is used in conjunction with calculateElapsedTime() to calulcate the total time requried for an operation to complete
*  PARAMETERS    : void: The function takes no arguments
*  RETURNS       : clock_t : Returns the number of clock ticks since the process was started 
*
*	NOTE: This function  was initially found online, however, the original soruce code has since been modified to suit the projects needs. 
		   As a result, partial credit belongs to the original authors on the website. For more information, please see the reference,
		   GeeksForGeeks.(ND). How to measure time taken by a function in C?. Retrieved on January 8, 2019, 
			from https://www.geeksforgeeks.org/how-to-measure-time-taken-by-a-program-in-c/
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
				   clock_t endTime	 : End time for when the transmission had finished
*  RETURNS       : double : Returns the elepased time between the two clock_t values
*/
double calculateElapsedTime(clock_t startTime, clock_t endTime)
{
	double elapsedTime = ((double)(endTime - startTime)) / CLOCKS_PER_SEC;
	return elapsedTime;

}//Done