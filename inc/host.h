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
#define SUCCESS        0
#define ERROR          -1
#define NUM_OF_CL_ARGS 4 
#define LEADER         7
#define MEMBER         8
#define NUM_OF_THREADS 4
#define LEADER_STRING  "leader"
#define MEMBER_STRING  "member"

/*
 * Global variables
 */
int udp;                               // UDP socket descriptor
struct sockaddr_in hostAddress;        // Host address

/* 
 * Function Declarations
 */
int CLA_checker(
                int argc,        // Number of CLA
                char *argv[]     // CLAs
                );
int setUpUDP();

/*
 * End 
 */
