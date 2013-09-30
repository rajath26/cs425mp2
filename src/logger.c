//////////////////////////////////////////////////////////////////////////////
//****************************************************************************
//
//    FILE NAME: logger.c
//
//    DECSRIPTION: This is the source file for logger functionality  
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
#include "logger.h"

/* 
 * Function definitions
 */

/*****************************************************************
 * NAME: logFileCreate 
 *
 * DESCRIPTION: Function returns file pointer to log file on the 
 *              host  
 *              
 * PARAMETERS: 
 *            (FILE *) fp - file pointer
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int logFileCreate(FILE *fp)
{

    int rc = SUCCESS;        // Return code 
       
    fp = fopen(LOG_FILE_LOCATION, "w");
    if ( NULL == fp ) 
    {
        printf("\nUnable to open log file in write mode\n");
        rc = ERROR;
        goto rtn;
    }
   
  rtn:
    return rc;
 
} // End of logFileCreate()

/*****************************************************************
 * NAME: printToLog 
 *
 * DESCRIPTION: Function takes care of writing into log 
 *              
 * PARAMETERS: 
 *            (FILE *) fp - file pointer
 *            (char *) keyMsg - buffer to be written as key
 *            (char *) valueMsg - buffer to be written as value
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int printToLog(FILE *fp, char *keyMsg, char *valueMsg)
{

    int rc = SUCCESS;        // Return code

    fprintf(fp, "%s : %s", keyMsg, valueMsg); 

  rtn:
    return rc;

} // End if printLog()

/*****************************************************************
 * NAME: logFileClose
 *
 * DESCRIPTION: Function takes care of closing log 
 *              
 * PARAMETERS: 
 *            (FILE *) fp - file pointer
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int logFileClose(FILE *fp)
{

    int rc = SUCCESS;

    fclose(fp);

  rtn:
    return rc;

} // End of logFileClose()

/*
 * End
 */
