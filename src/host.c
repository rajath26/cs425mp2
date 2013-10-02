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

    pthread_t tid = pthread_self();   // Thread ID

    sprintf(logMsg, "This is thread with counter: %d and thread ID: %u", counter, tid);
    printToLog(log, ipAddress, logMsg);

    switch(*counter)
    {
        case 1:
        // First thread calls receiver function that does:
        // i) Approve join requests if LEADER
        // ii) Receive heartbeats
        strcat(logMsg, "executing receiverFunc");
        printToLog(log, ipAddress, logMsg);
        i_rc = receiverFunc(); 
        break;

        case 2:
        // Second thread calls sender function that does:
        // i) Sends heartbeats
        strcat(logMsg, "executing senFunc");
        printToLog(log, ipAddress, logMsg);
        i_rc = sendFunc();
        break;

        case 3:
        // Third thread calls heartbeat checker function that:
        // i) checks heartbeat table
        strcat(logMsg, "executing heartBeatCheckerFunc");
        printToLog(log, ipAddress, logMsg);
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

/****************************************************************
 * NAME: receiverFunc 
 *
 * DESCRIPTION: This is the function that takes care of servicing
 *              the first among the three threads spawned i.e.
 *              the receiver threads which does the following:
 *              i) If I am a LEADER approve join requests from 
 *                 member hosts
 *              ii) Receive heartbeats
 *              
 * PARAMETERS: NONE 
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int receiverFunc()
{

    funcEntry(log, ipAddress, "receiverFunc");

    int rc = SUCCESS,                    // Return code
        numOfBytesRec,                   // Number of bytes received
        i_rc,                            // Temp RC
        op_code;                         // Operation code

    char recMsg[LONG_BUF_SZ],            // Received message
         tokenRecMsg[LONG_BUF_SZ];       // Received message without join op code 

    struct sockaddr_in memberAddress;    // Address of host 
 
    socklen_t length;                    // Length

    struct hb_entry * recMsgStruct;      // Heart beat table that holds received message

    recMsgStruct = (struct hb_entry *) malloc(sizeof(struct hb_entry));

    /*
     * 1) Receive UDP packet
     * 2) Check operation code
     * 3) If JOIN message: 
     *    i) Extract message
     *    ii) Update heartbeat table
     * 4) Else
     *    i) Extract message 
     *    ii) Update heartbeat table
     */

    for(;;)
    {
        length = sizeof(memberAddress);
        /////////
        // Step 1
        /////////
        numOfBytesRec = recvUDP(recMsg, LONG_BUF_SZ, memberAddress, &length);
        // Check if 0 bytes is received 
        if ( SUCCESS == numOfBytesRec )
        {
             sprintf(logMsg, "Number of bytes received is ZERO = %d", numOfBytesRec);
             printf("\n%s\n", logMsg);
             printToLog(log, ipAddress, logMsg);
             continue;
        }
        /////////
        // Step 2
        /////////
        i_rc = checkOperationCode(recMsg, op_code, tokenRecMsg);
        if ( i_rc != SUCCESS ) 
        {
            printToLog(log, ipAddress, "Unable to retrieve opcode");
            continue;
        }
        /////////
        // Step 3
        /////////
        if ( JOIN_OP_CODE == op_code )
        {
            sprintf(logMsg, "JOIN msg from %s", inet_ntop(AF_INET, &clientAddress.sin_addr, buffer, sizeof(buffer)));
            printToLog(log, ipAddress, logMsg);
            ///////////
            // Step 3i
            ///////////
            recMsgStruct = extract_message(tokenRecMsg);
            if ( NULL == recMsgStruct )
            {
                printToLog(log, ipAddress, "Unable to extract message");
                continue;
            }
            ////////////
            // Step 3ii
            ////////////
            i_rc = update_table(recMsgStruct);
            if ( i_rc != SUCCESS )
            {
                 printToLog(log, ipAddress, "Unable to update heart beat table");
                 continue;
            }
        } // End of if ( JOIN_OP_CODE == op_code )
        /////////
        // Step 4
        /////////
        else
        {
            //////////
            // Step 4i
            //////////
            recMsgStruct = extract_message(recMsg);
            if ( NULL == recMsgStruct )
            {
                printToLog(log, ipAddress, "Unable to extract message");
                continue;
            }
            ///////////
            // Step 4ii
            ///////////
            i_rc = update_table(recMsgStruct);
            if ( i_rc != SUCCESS )
            {
                 printToLog(log, ipAddress, "Unable to update heart beat table");
                 continue;
            }
        } // End of else
    } // End of for(;;)

  rtn:
    funcExit(log, ipAddress, "receiverFunc", rc);
    return rc;

} // End of receiverFunc()

/****************************************************************
 * NAME: checkOperationCode 
 *
 * DESCRIPTION: This is the function checks the passed in message
 *              and determines if it is a JOIN message or not
 *              
 * PARAMETERS: 
 * (char *) recMsg - received message
 * (int) op_code - pass by reference back to calling function
 *                 JOIN_OP_CODE if JOIN message else 
 *                 RECEIVE_HB_OP_CODE
 * (char *) tokenRecMsg - pass by reference back to calling 
 *                        function if JOIN_OP_CODE. Message with
 *                        JOIN OP CODE removed
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int checkOperationCode(char *recMsg, int op_code, char *tokenRecMsg)
{

    funcEntry(log, ipAddress, "checkOperationCode");
    
    int rc = SUCCESS;        // Return code
 
    char *token,             // Token
         joinDel = '$';      // JOIN message delimiter
 
    token = strtok(recMsg, joinDel);

    if ( NULL != token ) 
    {
        printToLog(log, ipAddress, "JOIN Op");
        op_code = JOIN_OP_CODE;
        token = strtok(NULL, joinDel); 
        strcpy(tokenRecMsg, token);
        printToLog(log, ipAddress, tokenRecMsg);
    }
    else
    {
        op_code = RECEIVE_HB_CODE;
        printToLog(log, ipAddress, "RECEIVE HB Op");
        printToLog(log, ipAddress, recMsg);
    }
    
  rtn:
    funcExit(log, ipAddress, "checkOperationCode", rc);
    return rc;

} // End of checkOperationCode()

/****************************************************************
 * NAME: sendFunc 
 *
 * DESCRIPTION: This is the function that takes care of sending
 *              heartbeats 
 *              
 * PARAMETERS: NONE 
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int sendFunc()
{

    funcEntry(log, ipAddress, "sendFunc");

    int rc = SUCCESS,                      // Return code
        num_of_hosts_chosen,               // Number of hosts chosen 
        i_rc,                              // Temp RC
        numOfBytesSent,                    // Number of bytes sent
        portNo;                            // Port no

    register int counter;                  // Counter

    char msgToSend[LONG_BUF_SZ],           // Message to be sent
         ipAddr[SMALL_BUF_SZ];             // IP Address buffer

    struct two_hosts hosts[GOSSIP_HOSTS],  // An array of two_hosts
           *ptr;                           // Pointer to above
 
    /*
    struct sockaddr_in hostAddress;        // Address of host to send HB
    */
 
    ptr = hosts;

    num_of_hosts_chosen = choose_n_hosts(ptr, GOSSIP_HOSTS);

    sprintf(logMsg, "Number of hosts chosen to gossip: %d", num_of_hosts_chosen);
    printToLog(log, ipAddress, logMsg);

    for ( counter = 0; counter < num_of_hosts_chosen; counter++ )
    {
        // Init chosen host address
        /*
        memset(&hostAddress, 0, sizeof(struct sockaddr_in));
        hostAddress.sin_family = AF_INET;
        hostAddress.sin_port = htons(hb_entry[hosts[counter].host_id].port);
        hostAddress.sin_addr.s_addr = inet_addr(hb_entry[hosts[counter].host_id].IP);
        memset(&(hostAddress.sin_zero), '\0', 8);
        */
        portNo = hb_entry[hosts[counter].host_id].port;
        strcpy(ipAddr, hb_entry[hosts[counter].host_id].IP);
        // create message
        i_rc = create_message(msgToSend);
        if ( SUCCESS != i_rc )
        {
            printToLog(log, ipAddress, "Unable to create message");
            continue;
        }

        // Send UDP packets
        numOfBytesSent = sendUDP(portNo, ipAddr, msgToSend);
        // check if 0 bytes is sent
        if ( SUCCESS == numOfBytesSent )
        {
            printToLog(log, ipAddress, "ZERO bytes sent");
            continue;
        }
    } // End of for ( counter = 0; counter < num_of_hosts_chosen; counter++ )
    
  rtn:
    funcExit(log, ipAddress, "sendFunc" rc);
    return rc;

} // End of sendFunc() 

/****************************************************************
 * NAME: heartBeatChecker 
 *
 * DESCRIPTION: This is the function that takes care of checking 
 *              heartbeats
 *              
 * PARAMETERS: NONE 
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int heartBeatChecker()
{

    funcEntry(log, ipAddress, "heartBeatChecker");

    int rc = SUCCESS,        // Return code
        i_rc;                // temp RC

    i_rc = periodic_heartbeat_update();
    if ( i_rc != SUCCESS )
    {
        printToLog(log, ipAddress, "periodic_heartbeat_update failed");
        continue;
    }

  rtn:
    funcExit(log, ipAddress, "heartBeatChecker", rc);
    return rc;

} // End of heartBeatChecker()


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


  rtn:
    funcExit(log, ipAddress, "Host::main", rc);
    /*
     * Close the log
     */ 
    logFileClose(log);
    return rc;

} // End of main
