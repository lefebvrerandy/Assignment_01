/*
*  FILE          : shared.c
*  PROJECT       : CNTR 2115 - Assignment #1
*  PROGRAMMER    : Randy Lefebvre & Bence Karner
*  FIRST VERSION : 2019-01-08
*  DESCRIPTION   : This file contains DEBUG
*
*  NOTE: DEBUG ADD THE REFERENCE TO NORBERTS PREVIOUS WORK, AND THE ONLINE POST HE GOT IT FROM
*/


#include "shared.h"


/*
*  FUNCTION      : createSocket
*  DESCRIPTION   : This function is used to create and initialze a socket with the appropriate properties as set by the parameters
*  PARAMETERS    : DEBUG
*  RETURNS       : SOCKET : Returns an initialized socket
*/
SOCKET createSocket(int protocolDomain, int socketType, int protocolType)
{
	SOCKET newSocket = socket(protocolDomain, socketType, protocolType);
	return newSocket;

}//Done