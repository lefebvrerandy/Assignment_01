/*
*  FILE          : server.c
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains DEBUG
*
*  NOTE: DEBUG ADD THE REFERENCE TO NORBERTS PREVIOUS WORK, AND THE ONLINE POST HE GOT IT FROM
*/


#include "server.h";
#include "shared.h";


/*
*  FUNCTION      : start_server
*  DESCRIPTION   : DEBUG
*  PARAMETERS    : DEBUG
*  RETURNS       : int :
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

	return 0;
}


/*
*  FUNCTION      : start_server_protocol
*  DESCRIPTION   : DEBUG
*  PARAMETERS    : DEBUG
*  RETURNS       : DEBUG
*/
int start_server_protocol(int tcp_or_udp)
{
	int networkResult = 0;				//Denotes the success or failure of the servers operation
	SOCKET acceptedSocketConnection;	//Socket used for connecting to the clients


	//Create local socket
	SOCKET openSocketHandle = createSocket(AF_INET, SOCK_STREAM, tcp_or_udp);
	if (openSocketHandle == INVALID_SOCKET)
	{
		perror("[ERROR]: Could not create socket");
		networkResult = ERROR;

	}
	else
	{

		//Clear the socket address variable, and initialize it's fields
		struct sockaddr_in socketAddress;
		memset((void*)&socketAddress, 0, sizeof(socketAddress));
		socketAddress.sin_family = AF_INET;								//Address family internet protocol
		socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);				//Convert from host byte order to network byte order
		socketAddress.sin_port = htons(storedData[2]);					//Port defined by CLA


		//Bind to the open socket
		int boundSocketHandle = bind(openSocketHandle, (struct sockaddr*)&socketAddress, sizeof(socketAddress));
		if (boundSocketHandle == SOCKET_ERROR)
		{
			perror("[ERROR]: Could not bind to the socket");
			networkResult = ERROR;

		}
		else
		{

			//Listen for an incominig connection to the open socket
			boundSocketHandle = listen(openSocketHandle, SOMAXCONN);
			if (boundSocketHandle == SOCKET_ERROR)
			{
				perror("[ERROR]: Could not listen to the socket");
				networkResult = ERROR;

			}
			else
			{

				//Accept the incoming client connection
				struct sockaddr_in remoteAddress;
				socklen_t addressLength = sizeof(remoteAddress);
				acceptedSocketConnection = accept(openSocketHandle, (struct sockaddr*)&remoteAddress, &addressLength);
				if (INVALID_SOCKET == acceptedSocketConnection)
				{
					perror("[ERROR]: Could not accept new connection");
					networkResult = ERROR;

				}
				else
				{

					//Send a reply to the client
					char outboundMessage[] = "serverReply\0";
					char recievedMessage[MESSAGE_BUFFER_SIZE] = "";
					memset((void*)recievedMessage, 0, sizeof(recievedMessage));
					send(acceptedSocketConnection, outboundMessage, strlen(outboundMessage), 0);


					//Recieve the clients reply
					recv(acceptedSocketConnection, recievedMessage, sizeof(recievedMessage), 0);
					printf("Client returned: %s\n", recievedMessage);
				}
			}
		}
	}

	//Clean up before exiting
	closesocket(acceptedSocketConnection);
	closesocket(openSocketHandle);
	return networkResult;
}