//////////////////////////////////////////////////////////////////////////////
//****************************************************************************
//
//    FILE NAME: udp.c
//
//    DECSRIPTION: This is the source file for UDP functionality  
//
//    OPERATING SYSTEM: Linux UNIX only
//    TESTED ON:
//
//    CHANGE ACTIVITY:
//    Date        Who      Description
//    ==========  =======  ===============
//    10-01-2013  Rajath   Initial creation
//
//****************************************************************************
//////////////////////////////////////////////////////////////////////////////

/*
 * Header files
 */
#include "udp.h"

/*
 * Function declarations
 */

/*****************************************************************
 * NAME: recvUDP 
 *
 * DESCRIPTION: This function is wrapper over recvFrom 
 *              
 * PARAMETERS: 
 * (char *) buffer - buffer received to be sent back 
 * (int) length - max length of buffer received 
 * (struct sockaddr_in) hostAddr - struct holding address of 
 *                                 host buffer received from
 * (socklen_t) - struct length

 * RETURN:
 * (int) bytes received 
 * 
 ****************************************************************/
int recvUDP(char *buffer, int length, struct sockaddr_in hostAddr, socklen_t sock_length)
{

    funcEntry(log, ipAddress, "recvUDP");
    
    int numOfBytesRec;       // Number of bytes received

    numOfBytesRec = recvFrom(udp, buffer, length, (struct sockaddr *) &hostAddr, &sock_length);

  rtn:
    funcExit(log, ipAddress, "recvUDP", numOfBytesRec);

} // End of recvUDP()

/*****************************************************************
 * NAME: sendUDP 
 *
 * DESCRIPTION: This function is wrapper over sendTo 
 *              
 * PARAMETERS: 
 * (int) portNo - port no
 * (char *) ipAddr - IP 
 * (char *) buffer - buffer to be sent 
 * 
 * RETURN:
 * (int) bytes sent 
 * 
 ****************************************************************/
int sendUDP(int portNo, char * ipAddr, char * buffer)
{

    funcEntry(log, ipAddress, "sendUDP");

    int numOfBytesSent;                 // Number of bytes sent 

    struct sockaddr_in hostAddr;        // Address of host to send message

    memset(hostAddr, 0, sizeof(struct sockaddr_in));
    hostAddr.sin_family = AF_INET;
    hostAddr.sin_port = htons(portNo);
    hostAddr.sin_addr.s_addr = inet_addr(ipAddr);
    memset(&(hostAddr.sin_zero), '\0', 8);

    numOfBytesSent = sendTo(udp, buffer, strlen(buffer), (struct sockaddr *) &hostAddr, sizeof(struct sockaddr));

  rtn:
    funcExit(log, ipAddress, "sendUDP", numOfBytesSent);
    return numOfBytesSent; 

} // End of sendUDP()

/*
 * End
 */
