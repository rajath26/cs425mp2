/////////////////////////////////////////////////////////////////////////////
//****************************************************************************
//
//    FILE NAME: host.c
//
//    DECSRIPTION: This is the source file for the leader host 
//                 and the member host
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

#include "../inc/host.h"
//#include "logger.c"
#include "udp.c"
#include "message_table.c"

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

    funcEntry(logF, ipAddress, "CLA_checker");

    int rc = SUCCESS;        // Return code

    if ( argc != NUM_OF_CL_ARGS )
    {
        printf("\nInvalid usage\n");
        printf("\nUsage information: %s <port_no> <ip_address>", argv[0]);
        rc = ERROR;
        goto rtn;
    }
    
  rtn:
    funcExit(logF, ipAddress, "CLA_checker", rc);
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

    funcEntry(logF, ipAddress, "setUpUDP");
    
    int rc = SUCCESS,        // Return code
        i_rc;                // Temp RC
 
    // Create a socket
    udp = socket(AF_INET, SOCK_DGRAM, 0);
    if ( ERROR == udp )
    {
        printf("\nUnable to open socket\n");
        printf("\nError number: %d\n", errno);
        printf("\nExiting.... ... .. . . .\n");
        perror("socket");
        printToLog(logF, ipAddress, "socket() failure");
        rc = ERROR;
    }
  
    printToLog(logF, ipAddress, "socket() successful");

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
        printToLog(logF, ipAddress, "bind() failure");
        rc = ERROR;
     }

     printToLog(logF, ipAddress, "bind() successful");

  rtn:
    funcExit(logF, ipAddress, "setUpUDP", rc);
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
int requestMembershipToLeader(int leaderPort, char *leaderIp)
{

    funcEntry(logF, ipAddress, "requestMembershipToLeader");

    int rc = SUCCESS,                            // Return code 
        i_rc,                                    // Temp RC
        numOfBytesSent;                          // Num of bytes sent

    char joinMessage[LONG_BUF_SZ],                // Buffer
         joinOperation[SMALL_BUF_SZ] = "JOIN$",  // Join prefix
         tableMessage[LONG_BUF_SZ];              // Table msg

    /*
     * Construct join message
     */
    printToLog(logF, ipAddress, "Message to be sent leader node is:");
    i_rc = create_message(tableMessage);
    sprintf(joinMessage, "%s%s", joinOperation, tableMessage);
    printToLog(logF, ipAddress, joinMessage);
    printToLog(logF, ipAddress, "Sending message to leader node");
    numOfBytesSent = sendUDP(leaderPort, leaderIp, joinMessage);
    sprintf(logMsg, "Num of bytes of join msg sent to leader: %d", numOfBytesSent);
    printToLog(logF, ipAddress, logMsg);
    // If number of bytes sent is 0
    if ( SUCCESS == numOfBytesSent)
    {
        rc = ERROR; 
        goto rtn;
    }
    printToLog(logF, ipAddress, "Join message sent successfully");

  rtn:
    funcExit(logF, ipAddress, "requestMembershipToLeader", rc);
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

    funcEntry(logF, ipAddress, "CLI_UI");

    int rc = SUCCESS,                    // Return code
        i_rc,                            // Temp RC
        leaderPortNo;                    // Leader port no

    char leaderIpAddress[SMALL_BUF_SZ];  // Buffer to hold leader ip
    
    printf("\n");
    printf("\t\t***********************************************************\n");
    printf("\t\t***********************************************************\n");
    printf("\t\tI am a Member host wanting to join Daisy distributed system\n");
    printf("\t\t***********************************************************\n");
    printf("\t\t***********************************************************\n");
    printf("\n\t\tInput the IP address of the Leader node:\n");
    scanf("%s", leaderIpAddress);
    printf("\n\t\tInput the Port No of the Laeder node:\n");
    scanf("%d", &leaderPortNo);
    sprintf(logMsg, "Trying to join %s at %d", leaderIpAddress, leaderPortNo);
    printToLog(logF, ipAddress, logMsg);
    i_rc = requestMembershipToLeader(leaderPortNo, leaderIpAddress);
    if ( i_rc != SUCCESS )
    {
        rc = ERROR;
        goto rtn;
    }
  
  rtn:
    funcExit(logF, ipAddress, "CLI_UI", rc);
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
   
    funcEntry(logF, ipAddress, "spawnHelperThreads");

    int rc = SUCCESS,                    // Return code
        i_rc,                            // Temp RC
        threadNum = 0,                   // Thread counter
        *ptr;                            // Pointer to thread counter
    
    register int counter;                // Counter variable
  
    pthread_t threadID[NUM_OF_THREADS];  // Helper threads

    /*
     * Create threads:
     */
    for ( counter = 0; counter < NUM_OF_THREADS; counter++ )
    {
        ptr = (int *) malloc(sizeof(int));
        *ptr = counter;
        // Debug. Uncomment if not req
        printf("\nptr : %d\n", *ptr);
        i_rc = pthread_create(&threadID[counter], NULL, startKelsa, (void *) ptr); 
        if ( SUCCESS != i_rc )
        {
            printf("\npthread creation failure\n");
            printf("\nError ret code: %d, errno: %d\n", i_rc, errno);
            printf("\nExiting.... ... .. . . .\n");
            rc = ERROR;
            printToLog(logF, ipAddress, "pthread() failure");
            goto rtn;
        }
        printToLog(logF, ipAddress, "pthread() success");
    }

    fclose(logF);
    for ( counter = 0; counter < NUM_OF_THREADS; counter++ )
    {
        pthread_join(threadID[counter], NULL);
    }

  rtn:
    funcExit(logF, ipAddress, "spawnHelperThreads", rc);
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

    funcEntry(logF, ipAddress, "startKelsa");

    int rc = SUCCESS,                 // Return code
        i_rc,                         // Temp RC
        *counter;                     // Thread counter

    counter = (int *) malloc(sizeof(int));
    counter = (int *) threadNum;

    pthread_t tid = pthread_self();   // Thread ID

    sprintf(logMsg, "This is thread with counter: %d and thread ID: %lu", *counter, tid);
    printToLog(logF, ipAddress, logMsg);

    printf("\ncounter: %d", *counter);
    switch(*counter)
    {
        case 0:
        // First thread calls receiver function that does:
        // i) Approve join requests if LEADER
        // ii) Receive heartbeats
        strcat(logMsg, "executing receiverFunc");
        printToLog(logF, ipAddress, logMsg);
        i_rc = receiverFunc(); 
        break;

        case 1:
        // Second thread calls sender function that does:
        // i) Sends heartbeats
        strcat(logMsg, "executing senFunc");
        printToLog(logF, ipAddress, logMsg);
        i_rc = sendFunc();
        break;

        case 2:
        // Third thread calls heartbeat checker function that:
        // i) checks heartbeat table
        strcat(logMsg, "executing heartBeatCheckerFunc");
        printToLog(logF, ipAddress, logMsg);
        i_rc = heartBeatCheckerFunc();
        break;

        default:
        // Can't get here if we do then exit
        printToLog(logF, ipAddress, "default case. An error");
        rc = ERROR;
        goto rtn;
        break;
    } // End of switch

  rtn:
    funcExit(logF, ipAddress, "startKelsa", rc);
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

    funcEntry(logF, ipAddress, "receiverFunc");

    int rc = SUCCESS,                    // Return code
        numOfBytesRec,                   // Number of bytes received
        i_rc,                            // Temp RC
        op_code;                         // Operation code

    char recMsg[LONG_BUF_SZ],            // Received message
         tokenRecMsg[LONG_BUF_SZ],       // Received message without join op code 
         buffer[SMALL_BUF_SZ];           // Temp buffer

    struct sockaddr_in memberAddress;    // Address of host 
 
    struct hb_entry * recMsgStruct;      // Heart beat table that holds received message

    recMsgStruct = (struct hb_entry *) malloc(4*sizeof(struct hb_entry));

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
        /////////
        // Step 1
        /////////
        numOfBytesRec = recvUDP(recMsg, LONG_BUF_SZ, memberAddress);
        // Check if 0 bytes is received 
        if ( SUCCESS == numOfBytesRec )
        {
             sprintf(logMsg, "Number of bytes received is ZERO = %d", numOfBytesRec);
             printf("\n%s\n", logMsg);
             printToLog(logF, ipAddress, logMsg);
             continue;
        }
        /////////
        // Step 2
        /////////
        i_rc = checkOperationCode(recMsg, &op_code, tokenRecMsg);
        if ( i_rc != SUCCESS ) 
        {
            printToLog(logF, ipAddress, "Unable to retrieve opcode");
            continue;
        }
        /////////
        // Step 3
        /////////
        if ( JOIN_OP_CODE == op_code )
        {
            sprintf(logMsg, "JOIN msg from %s", inet_ntop(AF_INET, &memberAddress.sin_addr, buffer, sizeof(buffer)));
            printToLog(logF, ipAddress, logMsg);
            ///////////
            // Step 3i
            ///////////
            clear_temp_entry_table(recMsgStruct);
            recMsgStruct = extract_message(tokenRecMsg);
            if ( NULL == recMsgStruct )
            {
                printToLog(logF, ipAddress, "Unable to extract message");
                continue;
            }
            ////////////
            // Step 3ii
            ////////////
            i_rc = update_table(recMsgStruct);
            if ( i_rc != SUCCESS )
            {
                 printToLog(logF, ipAddress, "Unable to update heart beat table");
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
            clear_temp_entry_table(recMsgStruct);
            recMsgStruct = extract_message(recMsg);
            if ( NULL == recMsgStruct )
            {
                printToLog(logF, ipAddress, "Unable to extract message");
                continue;
            }
            ///////////
            // Step 4ii
            ///////////
            i_rc = update_table(recMsgStruct);
            if ( i_rc != SUCCESS )
            {
                 printToLog(logF, ipAddress, "Unable to update heart beat table");
                 continue;
            }
        } // End of else
    } // End of for(;;)

  rtn:
    funcExit(logF, ipAddress, "receiverFunc", rc);
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
int checkOperationCode(char *recMsg, int *op_code, char *tokenRecMsg)
{

    funcEntry(logF, ipAddress, "checkOperationCode");
    
    int rc = SUCCESS;              // Return code
 
    char *token;                   // Token
    //const char joinDel = '$';      // JOIN message delimiter
 
    token = strtok(recMsg, "$");

    if ( NULL != token ) 
    {
        printToLog(logF, ipAddress, "JOIN Op");
        *op_code = JOIN_OP_CODE;
        token = strtok(NULL, "$"); 
        strcpy(tokenRecMsg, token);
        printToLog(logF, ipAddress, tokenRecMsg);
    }
    else
    {
        *op_code = RECEIVE_HB_OP_CODE;
        printToLog(logF, ipAddress, "RECEIVE HB Op");
        printToLog(logF, ipAddress, recMsg);
    }
    
  rtn:
    funcExit(logF, ipAddress, "checkOperationCode", rc);
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

    funcEntry(logF, ipAddress, "sendFunc");

    int rc = SUCCESS,                      // Return code
        num_of_hosts_chosen,               // Number of hosts chosen 
        i_rc,                              // Temp RC
        numOfBytesSent,                    // Number of bytes sent
        portNo;                            // Port no

    register int counter;                  // Counter

    char msgToSend[LONG_BUF_SZ],           // Message to be sent
         ipAddr[SMALL_BUF_SZ],             // IP Address buffer
         portNoChar[SMALL_BUF_SZ];         // Port no

    struct two_hosts hosts[GOSSIP_HOSTS],  // An array of two_hosts
           *ptr;                           // Pointer to above
 
    ptr = hosts;

    while(1)
    {
    
    initialize_two_hosts(ptr);
    num_of_hosts_chosen = choose_n_hosts(ptr, GOSSIP_HOSTS);

    sprintf(logMsg, "Number of hosts chosen to gossip: %d", num_of_hosts_chosen);
    printToLog(logF, ipAddress, logMsg);

    for ( counter = 0; counter < num_of_hosts_chosen; counter++ )
    {
        strcpy(portNoChar, hb_table[hosts[counter].host_id].port);
        portNo = atoi(portNoChar);
        strcpy(ipAddr, hb_table[hosts[counter].host_id].IP);
        // create message
        i_rc = create_message(msgToSend);
        if ( SUCCESS != i_rc )
        {
            printToLog(logF, ipAddress, "Unable to create message");
            continue;
        }

        // Send UDP packets
        numOfBytesSent = sendUDP(portNo, ipAddr, msgToSend);
        // check if 0 bytes is sent
        if ( SUCCESS == numOfBytesSent )
        {
            printToLog(logF, ipAddress, "ZERO bytes sent");
            continue;
        }
    } // End of for ( counter = 0; counter < num_of_hosts_chosen; counter++ )
    sleep(2);
    } // End of while 
    
  rtn:
    funcExit(logF, ipAddress, "sendFunc", rc);
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
int heartBeatCheckerFunc()
{

    funcEntry(logF, ipAddress, "heartBeatChecker");

    int rc = SUCCESS;        // Return code

     while(1){
       sleep(HEART_BEAT_UPDATE_SEC);
       update_my_heartbeat();
       check_table_for_failed_hosts();
     }

  rtn:
    funcExit(logF, ipAddress, "heartBeatChecker", rc);
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

    /*
     * Init log file 
     */
    printf("\nCreating log\n");
    i_rc = logFileCreate(logF);
    if ( i_rc != SUCCESS )
    {
         printf("\nLog file won't be created. There was an error\n");
    }

    printf("\nlog success\n");

    /*
     * Copy ip address and port no to local buffer
     */
    memset(ipAddress, '\0', SMALL_BUF_SZ);
    sprintf(ipAddress, "%s", argv[2]);
    memset(portNo, '\0', SMALL_BUF_SZ);
    sprintf(portNo, "%s", argv[1]);


    printf("\nfuncEntry\n");
    funcEntry(logF, ipAddress, "host::main");
    printf("\nAfter funcENtery\n");

    /*
     * Command line arguments check
     */
    i_rc = CLA_checker(argc, argv);
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
    memset(portNo, '\0', SMALL_BUF_SZ);
    sprintf(portNo, "%s", argv[1]);

    printf("\n before init table\n");
    /*
     * Init local host heart beat table
     */
    initialize_table();
    printToLog(logF, ipAddress, "Initialized my table");
     printf("\n AFter init tabl\n");

    /* 
     * Get the node type based on third argument. By default it
     * is always member node.
     */
    if ( SUCCESS == strcmp(argv[3], LEADER_STRING) )
    {
        isLeader = true;
        printToLog(logF, ipAddress, "I am the leader node");
    }
    else 
    {
        printToLog(logF, ipAddress, "I am a member node");
    }

    /* 
     * Set up UDP 
     */
    i_rc = setUpUDP(portNo, ipAddress); 
    if ( i_rc != SUCCESS )
    {
        rc = ERROR;
        printToLog(logF, ipAddress, "UDP setup failure");
        goto rtn;
    }

    // Log current status 
    printToLog(logF, ipAddress, "UDP setup successfully");

    /*
     * If current host is a LEADER then log that this host has
     * joined the distributed system
     */
    if ( isLeader )
    {
        printToLog(logF, ipAddress, "I, THE LEADER have joined the Daisy Distributed System");
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
    funcExit(logF, ipAddress, "Host::main", rc);
    /*
     * Close the log
     */ 
    logFileClose(logF);
    return rc;

} // End of main
