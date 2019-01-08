/*
================
  This file is part of a tutorial written for writing sockets
  in C that are portable between Windows and Linux. It's meant
  to give a basic understanding of the underlying socket process
  and how to get a simple connection established over TCP and how
  to send/recieve basic packets.

  If this file is distributed, please include the full source package from
  http://www.webpak.net/~energon/
  It was given to you as a learning tool, so please pay it forward.
================
*/


// standard C headers
#include <string.h>
#include <stdio.h>


// include headers based on OS
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
    struct sockaddr_in addr, r_addr;  // address variables
    SOCKET    s, t;                   // sockets
    int       r;                      // to hold return values
    socklen_t len = sizeof(r_addr);   // the length of our remote address

    // create the local socket
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(INVALID_SOCKET == s) {
        perror("Could not create socket");
        return -1;
    }

    // setup the local address variable
    memset((void*)&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port        = htons(6868);

    // name the local socket
    r = bind(s, (struct sockaddr*)&addr, sizeof(addr));
    if(SOCKET_ERROR == r) {
        perror("Could not bind to local socket");
        closesocket(s);
        return -1;
    }

    // set the socket to listen for a connection
    r = listen(s, SOMAXCONN);
    if(SOCKET_ERROR == r) {
        perror("Could not listen to local socket");
        closesocket(s);
        return -1;
    }

    // a little user interaction... ;)
    printf("Waiting for connection... ");
    fflush(stdout);

    // wait for a connection
    t = accept(s, (struct sockaddr*)&r_addr, &len);
    if(INVALID_SOCKET == t) {
        perror("Could not accept new connection");
        closesocket(s);
        return -1;
    }
    printf("accepted.\n");

    // send some data to the newly connected client
    char data[] = "Hello client!\0";
    char recieved[256];
    memset((void*)recieved, 0, sizeof(recieved));
    send(t, data, strlen(data), 0);

    // wait for a reply
    recv(t, recieved, sizeof(recieved), 0);
    printf("Client returned: %s\n", recieved);

    // cleanup is VERY, VERY important with sockets!!!
    // they are file descriptors (and you only get so many in the life of an OS)
    closesocket(t);
    closesocket(s);

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
    // only one argument besides the actual command allowed
    if(2 == n) {
        if(0 == strcmp(args[1], "-s"))  // server
            return 1;
        else if(0 == strcmp(args[1], "-c"))  // client
            return 2;
    }

    return 0;
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
