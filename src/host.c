/////////////////////////////////////////////////////////////////////////////
//****************************************************************************
//
//    FILE NAME: host.c
//
//    DECSRIPTION: This is the source file for the leader host 
//                 i.e. the contact host that approves other hosts
//                 to join the network and the member host
//
//    OPERATING SYSTEM: Linux UNIX only
//    TESTED ON:
//
//    CHANGE ACTIVITY:
//    Date        Who      Description
//    ==========  =======  ===============
//    09-29-2013  Rajath   Initial creation
//
//****************************************************************************
//////////////////////////////////////////////////////////////////////////////

#include "host.h"

/*
 * Function definitions
 */

/*****************************************************************
 * NAME: CLA_checker 
 *
 * DESCRIPTION: This function is designed to check if command 
 *              line arguments is valid 
 *              
 * PARAMETERS: 
 *            (int) argc - number of command line arguments
 *            (char *) argv - two command line arguments apart 
 *                            from argv[0] namely:
 *                            i) Port No
 *                            ii) Ip Address of host
 *                            iii) Host Type 
 *                                 "leader" -> Leader Node
 *                                 "member" -> Member Node
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int CLA_checker(int argc, char *argv[])
{

    int rc = SUCCESS;        // Return code

    if ( argc != NUM_OF_CL_ARGS )
    {
        printf("\nInvalid usage\n");
        printf("\nUsage information: %s <port_no> <ip_address>", argv[0]);
        rc = ERROR;
        goto rtn;
    }
    
  rtn:
    return rc;

} // End of CLA_checker()

/*****************************************************************
 * NAME: setUpUDP 
 *
 * DESCRIPTION: This function is designed to create a UDP and 
 *              bind to the port 
 *              
 * PARAMETERS: 
 *            (char *) portNo: port number
 *            (char *) ipAddress: IP Address
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int setUpUDP(char * portNo, char * ipAddress)
{

    int rc = SUCCESS;
 
    // Create a socket
    udp = socket(AF_INET, SOCK_DGRAM, 0);
    if ( ERROR == udp )
    {
        printf("\nUnable to open socket\n");
        printf("\nError number: %d\n", errno);
        printf("\nExiting.... ... .. . . .\n");
        perror("socket");
        rc = ERROR;
    }

    memset(&hostAddress, 0, sizeof(struct sockaddr_in));
    hostAddress.sin_family = AF_INET;
    hostAddress.sin_port = htons(atoi(portNo)); 
    hostAddress.sin_addr.s_addr = inet_addr(ipAddress);
    memset(&(hostAddress.sin_zero), '\0', 8);

    // Bind the socket
    i_rc = bind(udp, (struct sockaddr *) &hostAddress, sizeof(hostAddress));
    if ( ERROR == i_rc )
    {
        printf("\nUnable to bind socket\n");
        printf("\nError number: %d\n", errno);
        printf("\nExiting.... ... .. . . .\n");
        perror("bind");
        rc = ERROR;
     }

  rtn:
    return rc;

} // End of setUpUDP()

/*****************************************************************
 * NAME: approverMemberHosts 
 *
 * DESCRIPTION: This function is designed to create a UDP and 
 *              bind to the port 
 *              
 * PARAMETERS: 
 *            (char *) portNo: port number
 *            (char *) ipAddress: IP Address
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/

/*
 * Main function
 */

/*****************************************************************
 * NAME: main 
 *
 * DESCRIPTION: Main function of the leader host i.e. the contact 
 *              host that approves other hosts to join the
 *              network and the member host. This binary is 
 *              invoked via a start up script. The parameters are
 *              port no and ip address and node type.
 *              
 * PARAMETERS: 
 *            (int) argc - number of command line arguments
 *            (char *) argv - two command line arguments apart 
 *                            from argv[0] namely:
 *                            i) Port No
 *                            ii) Ip Address of host
 *                            iii) Host Type 
 *                                 "leader" -> Leader Node
 *                                 "member" -> Member Node
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int main(int argc, char *argv[])
{

    int rc       = SUCCESS,        // Return code
        i_rc,                      // Intermittent return code
        udp;                       // Socket descriptor
        
    register int counter;          // Counter variable 

    bool isLeader = FALSE;         // Bool variable

    char ipAddress[SMALL_BUF_SZ],  // Buffer to hold ip 
         portNo[SMALL_BUF_SZ];     // Buffer to hold port no

    FILE *log;                     // File pointer to log 
                    
    pthread_t approverThread,              // Approver thread
              threadID[NUM_OF_THREADS];    // Helper threads

    /*
     * Command line arguments check
     */
    i_rc = CLA_checker(int argc, char *argv[]);
    if ( i_rc != SUCCESS )
    {
        rc = ERROR;
        goto rtn;
    }
       
    /*
     * Init log file 
     */
    i_rc = logFileCreate(log);
    if ( i_rc != SUCCESS )
    {
         printf("\nLog file won't be created. There was an error\n");
    }

    // Copy ip address and port no to local buffer
    memset(ipAddress, '\0', SMALL_BUF_SZ);
    sprintf(ipAddress, "%s", argv[2]);
    memset(portNo, '\0', SNALL_BUF_SZ);
    sprintf(portNo, "%s", argv[1]);

    /*
     * Init local host heart beat table
     */
    initialize_table();

    /* 
     * Get the node type based on third argument. By default it
     * is always member node.
     */
    if ( SUCCESS == strcmp(argv[3], LEADER_STRING) )
    {
        isLeader = TRUE;
    }

    /* 
     * Set up UDP 
     */
    i_rc = setUpUDP(portNo, ipAddress); 
    if ( i_rc != SUCCESS )
    {
        rc = ERROR;
        goto rtn;
    }

    /*
     * If this host is a leader spawn a thread that acts as an 
     * approver for other member hosts
     */ 
    if ( isLeader )
    {
        i_rc = pthread_create(&approverThread, NULL, approverMemberHosts, NULL);
        if ( i_rc != SUCCESS )
        {
            printf("\nThread creation failed\n");
            printf("\nError Number: %d\n", errno);
            perror("pthread_create");
            printf("\nExiting.... ... .. .\n");
            rc = ERROR;
            goto rtn;
        }
    }

    // If the current host is a LEADER then log that this host
    // has joined the distributed system
    // NOTE:- In all calls to printToLog return code is not 
    //        checked. It is assumed that it is always successful
    //        and logFileCreate was successful too
    if ( isLeader )
    {
        printToLog(log, ipAddress, "I have joined the distributed system");
    }
    else 
    {
    
    
    /*
     * Create threads:
     */
    for ( counter = 0; counter < NUM_OF_THREADS; counter++ )
    {
        i_rc = pthread_create(&threadID[counter], NULL, startKelsa, (void *) isLeader);
        if ( SUCCESS != i_rc )
        {
            printf("\npthread creation failure\n");
            printf("\nError ret code: %d, errno: %d\n", i_rc, errno);
            printf("\nExiting.... ... .. . . .\n");
            rc = ERROR;
            goto rtn;
        } 
    }
    
      


  rtn:
    return rc;

} // End of main
