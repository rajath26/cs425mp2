//////////////////////////////////////////////////////////////////////////////
//****************************************************************************
//
//    FILE NAME: host.h 
//
//    DECSRIPTION: This is the header file for the host that 
//                 functions as the leader i.e. the contact host
//                 that approves other hosts joining the cluster
//                 and the member hosts  
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

/*
 * Header files
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/time.h>

/*
 * Macros
 */
#define SUCCESS            0
#define ERROR              -1
#define NUM_OF_CL_ARGS     4 
#define LEADER             7
#define MEMBER             8
#define NUM_OF_THREADS     3 
#define JOIN_OP_CODE       9
#define RECEIVE_HB_OP_CODE 8
#define GOSSIP_HOSTS       2
#define LEADER_STRING      "leader"
#define MEMBER_STRING      "member"

/*
 * Global variables
 */
FILE *log;                             // File pointer to log
int udp;                               // UDP socket descriptor
struct sockaddr_in hostAddress;        // Host address
char ipAddress[SMALL_BUF_SZ],          // IP of current host
     portNo[SMALL_BUF_SZ],             // Port no of current host
     logMsg[MED_BUF_SZ];               // Log message buffer 
bool isLeader = FALSE;                 // Bool variable

/* 
 * Function Declarations
 */
int CLA_checker(
                int argc,        // Number of CLA
                char *argv[]     // CLAs
                );
int setUpUDP();
int requestMembershipToLeader(
                              char *leaderPort,   // Leader IP
                              char *leaderIp,     // Leader port
                             );
int CLI_UI();
int spawnHelperThreads();
void * startKelsa(void *);
int receiverFunc();
int sendFunc();
int heartBeatCheckerFunc();
int checkOperationCode(
                       char * recMsg,    // Received message
                       int op_code       // Returned op code
                      );

/*
 * End 
 */
