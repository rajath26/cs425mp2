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

    funcEntry(log, ipAddress, "CLA_checker");

    int rc = SUCCESS;        // Return code

    if ( argc != NUM_OF_CL_ARGS )
    {
        printf("\nInvalid usage\n");
        printf("\nUsage information: %s <port_no> <ip_address>", argv[0]);
        rc = ERROR;
        goto rtn;
    }
    
  rtn:
    funcExit(log, ipAddress, "CLA_checker", rc);
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

    funcEntry(log, ipAddress, "setUpUDP");
    
    int rc = SUCCESS;
 
    // Create a socket
    udp = socket(AF_INET, SOCK_DGRAM, 0);
    if ( ERROR == udp )
    {
        printf("\nUnable to open socket\n");
        printf("\nError number: %d\n", errno);
        printf("\nExiting.... ... .. . . .\n");
        perror("socket");
        printToLog(log, ipAddress, "socket() failure");
        rc = ERROR;
    }
  
    printToLog(log, ipAddress, "socket() successful");

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
        printToLog(log, ipAddress, "bind() failure");
        rc = ERROR;
     }

     printToLog(log, ipAddress, "bind() successful");

  rtn:
    funcExit(log, ipAddress, "setUpUDP", rc);
    return rc;

} // End of setUpUDP()

/*****************************************************************
 * NAME: requestMembershipToLeader 
 *
 * DESCRIPTION: This function is designed to let member host 
 *              request leader node membership to Daisy 
 *              distributed system
 *              
 * PARAMETERS: 
 *            (char *) leaderPort: leader port number
 *            (char *) leaderIp: leader IP Address
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int requestMembershipToLeader(char *leaderPort, char *leaderIp)
{

    funcEntry(log, ipAddress, "requestMembershipToLeader");

    int rc = SUCCESS,                            // Return code 
        i_rc,                                    // Temp RC
        numOfBytesSent; 

    char joinMessage[MED_BUF_SZ],                // Buffer
         joinOperation[SMALL_BUF_SZ] = "JOIN$",  // Join prefix
         tableMessage[LONG_BUF_SZ];              // Table msg

    /*
     * Construct join message
     */
    printToLog(log, ipAddress, "Message to be sent leader node is:");
    i_rc = create_message(tableMessage);
    sprintf(joinMessage, "%s%s", joinOperation, tableMessage);
    printToLog(log, ipAddress, joinMessage);
    printToLog(log, ipAddress, "Sending message to leader node");
    numOfBytesSent = sendUDP(leaderPort, leaderIp, joinMessage);
    sprintf(logMsg, "Num of bytes of join msg sent to leader: %d", numOfBytesSent);
    printToLog(log, ipAddress, logMsg);
    // If number of bytes sent is 0
    if ( SUCCESS == numOfBytesSent)
    {
        rc = ERROR; 
        goto rtn;
    }
    printToLog(log, ipAddress, "Join message sent successfully");

  rtn:
    funcExit(log, ipAddress, "requestMembershipToLeader", rc);
    return rc;

} // End of requestMembershipToLeader()

/*****************************************************************
 * NAME: CLI_UI 
 *
 * DESCRIPTION: This function is designed to display CLI UI for 
 *              member hosts
 *              
 * PARAMETERS: NONE
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int CLI_UI()
{

    funcEntry(log, ipAddress, "CLI_UI");

    int rc = SUCCESS;                    // Return code

    char leaderIpAddress[SMALL_BUF_SZ],  // Buffer to hold leader ip
         leaderPortNo[SMALL_BUF_SZ];     // Buffer to hold leader port no
    
    printf("\n");
    printf("\t\t***********************************************************\n");
    printf("\t\t***********************************************************\n");
    printf("\t\tI am a Member host wanting to join Daisy distributed system\n"0);
    printf("\t\t***********************************************************\n");
    printf("\t\t***********************************************************\n");
    printf("\n\t\tInput the IP address of the Leader node:\n");
    scanf("%s", &leaderIpAddress);
    printf("\n\t\tInput the Port No of the Laeder node:\n");
    scanf("%s", &leaderPortNo);
    sprintf(logMsg, "Trying to join %s at %s", leaderIpAddress, leaderPortNo);
    printToLog(log, ipAddress, logMsg);
    i_rc = requestMembershipToLeader(leaderPortNo, leaderIpAddress);
    if ( i_rc != SUCCESS )
    {
        rc = ERROR;
        goto rtn;
    }
  
  rtn:
    funcExit(log, ipAddress, "CLI_UI", rc);
    return rc;

} // End of CLI_UI()

/*****************************************************************
 * NAME: spawnHelperThreads 
 *
 * DESCRIPTION: This function spawns helper threads 
 *              
 * PARAMETERS: NONE
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int spawnHelperThreads()
{
   
    funcEntry(log, ipAddress, "spawnHelperThreads");

    int rc = SUCCESS,       // Return code
        i_rc,               // Temp RC
        threadNum = 0,      // Thread counter
        *ptr;               // Pointer to thread counter
    
    register int counter;   // Counter variable

    ptr = (int *) malloc(sizeof(int));

    /*
     * Create threads:
     */
    for ( counter = 0; counter < NUM_OF_THREADS; counter++ )
    {
        *ptr = threadNum;
        i_rc = pthread_create(&threadID[counter], NULL, startKelsa, (void *) ptr); 
        if ( SUCCESS != i_rc )
        {
            printf("\npthread creation failure\n");
            printf("\nError ret code: %d, errno: %d\n", i_rc, errno);
            printf("\nExiting.... ... .. . . .\n");
            rc = ERROR;
            printToLog(log, ipAddress, "pthread() failure");
            goto rtn;
        }
        printToLog(log, ipAddress, "pthread() success");
        threadNum++;
    }

  rtn:
    funcExit(log, ipAddress, "spawnHelperThreads", rc);
    return rc;
} // End of spawnHelperThreads();

/****************************************************************
 * NAME: startKelsa 
 *
 * DESCRIPTION: This is the pthread function 
 *              
 * PARAMETERS: 
 * (void *) threadNum - thread counter
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
void * startKelsa(void *threadNum)
{

    funcEntry(log, ipAddress, "startKelsa");

    int rc = SUCCESS,                 // Return code
        i_rc,                         // Temp RC
        *counter = (int *)threadNum;  // Thread counter

    switch(*counter)
    {
        case 1:
        // First thread calls receiver function that does:
        // i) Approve join requests if LEADER
        // ii) Receive heartbeats
        i_rc = receiverFunc(); 
        break;

        case 2:
        // Second thread calls sender function that does:
        // i) Sends heartbeats
        i_rc = sendFunc();
        break;

        case 3:
        // Third thread calls heartbeat checker function that:
        // i) checks heartbeat table
        i_rc = heartBeatCheckerFunc();
        break;

        default:
        // Can't get here if we do then exit
        printToLog(log, ipAddress, "default case. An error");
        rc = ERROR;
        goto rtn;
        break;
    } // End of switch

  rtn:
    funcExit(log, ipAddress, "startKelsa", rc);
    return rc;
}

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

    int rc = SUCCESS,              // Return code
        i_rc;                            // Intermittent return code
        
    char leaderIpAddress[SMALL_BUF_SZ],  // Buffer to hold leader ip
         leaderPortNo[SMALL_BUF_SZ];     // Buffer to hold leader port no

    pthread_t threadID[NUM_OF_THREADS];  // Helper threads

    /*
     * Init log file 
     */
    i_rc = logFileCreate(log);
    if ( i_rc != SUCCESS )
    {
         printf("\nLog file won't be created. There was an error\n");
    }

    funcEntry(log, ipAddress, "host::main");

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
     * Copy ip address and port no to local buffer
     */
    memset(ipAddress, '\0', SMALL_BUF_SZ);
    sprintf(ipAddress, "%s", argv[2]);
    memset(portNo, '\0', SNALL_BUF_SZ);
    sprintf(portNo, "%s", argv[1]);

    /*
     * Init local host heart beat table
     */
    initialize_table();
    printToLog(log, ipAddress, "Initialized my table");

    /* 
     * Get the node type based on third argument. By default it
     * is always member node.
     */
    if ( SUCCESS == strcmp(argv[3], LEADER_STRING) )
    {
        isLeader = TRUE;
        printToLog(log, ipAddress, "I am the leader node");
    }
    else 
    {
        printToLog(log, ipAddress, "I am a member node");
    }

    /* 
     * Set up UDP 
     */
    i_rc = setUpUDP(portNo, ipAddress); 
    if ( i_rc != SUCCESS )
    {
        rc = ERROR;
        printfToLog(log, ipAddress, "UDP setup failure");
        goto rtn;
    }

    // Log current status 
    printToLog(log, ipAddress, "UDP setup successfully");

    /*
     * If current host is a LEADER then log that this host has
     * joined the distributed system
     */
    if ( isLeader )
    {
        printToLog(log, "I, THE LEADER have joined the Daisy Distributed System");
    }

    /*
     * Display the CLI UI if this host is a MEMBER host which 
     * asks member if he wants to send join message to leader node
     * and calls the function requestMembershipToLeader() which
     * does the job
     */
    if ( !isLeader )
    {
        i_rc = CLI_UI();
        if ( i_rc != SUCCESS )
        {
            rc = ERROR;
            goto rtn;
        }
    }

    /*
     * Spawn the helper threads
     */
    i_rc = spawnHelperThreads();
    if ( i_rc != SUCCESS )
    {
        rc = ERROR;
        goto rtn;
    }
    
    
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
    funcExit(log, ipAddress, "Host::main", rc);
    /*
     * Close the log
     */ 
    logFileClose(log);
    return rc;

} // End of main
