//////////////////////////////////////////////////////////////////////////////
//****************************************************************************
//
//    FILE NAME: logger.h 
//
//    DECSRIPTION: This is the header file for logger functionality  
//
//    OPERATING SYSTEM: Linux UNIX only
//    TESTED ON:
//
//    CHANGE ACTIVITY:
//    Date        Who      Description
//    ==========  =======  ===============
//    09-28-2013  Rajath   Initial creation
//
//****************************************************************************
//////////////////////////////////////////////////////////////////////////////

/*
 * Header files
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * Macros
 */
#define LOG_FILE_LOCATION "~/machine.log"
#define ERROR             -1
#define SUCCESS           0

/*
 * Function Declarations
 */
int logFileCreate(
                  FILE *fp        // File pointer 
                 );
int printToLog(
             FILE *fp,            // File pointer 
             char *keyMessage,    // Message to be written as key 
             char *valueMessage   // Message to be written as value 
            ); 
int logFileClose(
                 FILE *fp         // File pointer 
                );

/*
 * End 
 */