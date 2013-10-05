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
#include "../inc/udp.h"

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
int recvUDP(char *buffer, int length, struct sockaddr_in hostAddr)
{

    funcEntry(logF, ipAddress, "recvUDP");

    int numOfBytesRec;       // Number of bytes received

    socklen_t len;           // Length

    len = sizeof(hostAddr);

    numOfBytesRec = recvfrom(udp, buffer, length, 0, (struct sockaddr *) &hostAddr, &len);
    network_to_host(buffer);
    // Debug. uncomment if required
    printf("\nbuffer\n");

  rtn:
    funcExit(logF, ipAddress, "recvUDP", numOfBytesRec);
    return numOfBytesRec;

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

    funcEntry(logF, ipAddress, "sendUDP");

    int numOfBytesSent;                 // Number of bytes sent 

    struct sockaddr_in hostAddr;        // Address of host to send message
    int value;
    memset(&hostAddr, 0, sizeof(struct sockaddr_in));
    hostAddr.sin_family = AF_INET;
    hostAddr.sin_port = htons(portNo);
    hostAddr.sin_addr.s_addr = inet_addr(ipAddr);
    memset(&(hostAddr.sin_zero), '\0', 8);
    static int k=-1;
    static int n=-1;
    static int random[10];
    static int vis[100];
    int rand_percent=10;
    // choose ten random numbers when k=-1 
    int i;
    if(k==-1){
         for(i=0;i<100;i++) vis[i]=0;
    }
    if(k==-1){
         while(1){
            k++; 
            for(i=0;i<100;i++) vis[i]=0; 
            if(k==rand_percent) break;
          random:  value = rand()%100;
            if(vis[value]==0){
                    vis[value]=1;
                    random[k]=value;
                    k++; 
            }
            else {goto random;}
         } 
    }
    n++;   
    if(n==100){
         k=-1;
         n=0;
         for(i=0;i<rand_percent;i++){
              random[i]=0;
         }
         for(i=0;i<100;i++)vis[i]=0;  
    }
    for(i=0;i<rand_percent;i++){
        if(n==random[i]){
               random[i]=-1;
               goto rtn;
        }     
   }          
               
    host_to_network(buffer);
    numOfBytesSent = sendto(udp, buffer, strlen(buffer), 0, (struct sockaddr *) &hostAddr, sizeof(struct sockaddr));

rtn:
    funcExit(logF, ipAddress, "sendUDP", numOfBytesSent);
    return numOfBytesSent; 

} // End of sendUDP()

/*
 * End
 */
