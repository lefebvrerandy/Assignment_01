/*
*  FILE          : server.c
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains all the functions required to operate the sever component of the application.
*				   Functions are included for adding multi thread support, opening TCP & UDP sockets, and sending/recieving 
*				   messages from the clients
*/


#include "shared.h"
#include "client.h"
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

	//Spawn two threads. One for TCP, one for UDP
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
		// An error has occurred
		perror("Could not create Thread.");
	}
	else if (pthread_create(&thread_linux_server[1], NULL, start_server_protocol, (void*)IPPROTO_UDP) != 0)
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
*	int stream_or_datagram : Denotes if thes socket is of type SOCK_STREAM or SOCK_DGRAM
*	int tcp_or_udp		   : Denotes if the protocol is DEBUG
*  RETURNS       : int : Returns an integer indicating the functions success (ie. return > 0) or failure (ie. return < 0)
*/
int start_server_protocol(int stream_or_datagram, int tcp_or_udp)
{
	int networkResult = 1;				//Denotes the success or failure of the servers operation
	SOCKET acceptedSocketConnection;	//Socket used for connecting to the clients



	//Stage 1: Create local socket
	SOCKET openSocketHandle = createSocket(AF_INET, SOCK_STREAM, tcp_or_udp);	//SET THE PARAMETER FOR THE FUNCTION TO INCLUDE SOCK_STREAM or SOCK_DGRAM
	if (openSocketHandle == INVALID_SOCKET)
	{
		networkResult = setErrorState(SOCKET_CREATION_ERROR);					//Set return to -1, and print an error for the stage of connection
	}
	else
	{
		struct sockaddr_in socketAddress;
		memset((void*)&socketAddress, 0, sizeof(socketAddress));				//Clear the address struct for initialization
		


		//Stage 2A: Initialize the socket struct
		socketAddress.sin_family = AF_INET;										//Address family internet protocol
		socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);						//Convert from host byte order to network byte order
		socketAddress.sin_port = htons(storedData[2]);							//Port defined by CLA


		//Print server connection info to the screen		// NEEDS TESTING
		char hostbuffer[256];
		char *IPbuffer;
		struct hostent *host_entry;
		int hostname;

		char hostPort[10];
		strcpy(hostPort, socketAddress.sin_port);

		hostname = gethostname(hostbuffer, sizeof(hostbuffer));
		host_entry = gethostbyname(hostbuffer);
		// To convert an Internet network 
		// address into ASCII string 
		IPbuffer = inet_ntoa(*((struct in_addr*)
			host_entry->h_addr_list[0]));

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
				struct sockaddr_in remoteAddress;


				//Stage 4: Accept the incoming client connection
				socklen_t addressSize = sizeof(remoteAddress);
				acceptedSocketConnection = accept(openSocketHandle, (struct sockaddr*)&remoteAddress, &addressSize);
				if (acceptedSocketConnection == INVALID_SOCKET)
				{
					networkResult = setErrorState(SOCKET_CONNECTION_ERROR);		//Set return to -1, and print an error for the stage of connection
				}
				else
				{
					//Stage 5: Send a reply to the client
					//Stage 6: Receive the clients reply


				}
			}
		}
	}


	//Clean up before exiting
	closesocket(acceptedSocketConnection);
	closesocket(openSocketHandle);
	return networkResult;
}