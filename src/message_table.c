#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"logger.c"
#include"message.h"
/////////////////////////////////////////////////////////////////////////////////////
//                FILE NAME : message_table.c                                      //
//                              						   //	
// Description : This file provides the following functionalities                  //
//                 * Provides interfaces to serialize and de-serialize messages    //
//                 * Provides interfaces to convert messages to machine independent//
//                   forms.						           //		
//                 * Provides interfaces to perform various operation on a table   //
//                 * Provides interfaces to update the table, delete an entry and  //
//                   initialize a table.                                           //
/////////////////////////////////////////////////////////////////////////////////////


char logMsg1[500];   // Global variable to send the log messages to the logger interface



/*
 *  This function is used for deleting an entry in the table
 *  input param : index of the table that needs to be deleted
 */

int delete_entry_table(int table_index)
{
   funcEntry(logF,ip_Address,"delete_entry_table");
   memset(&hb_table[table_index],0,sizeof(struct hb_entry));
   sprintf(logMsg1,"host_entry %d in the hb_table is deleted",table_index);
   printToLog(logF,ip_Address,logMsg1);
   funcExit(logF,ip_Address,"delete_entry_table",0);
   return 0;
}

/*
 * This function provides functionality to clear the table entries
 * input param : table which needs to be removed
 */

int clear_temp_entry_table(struct hb_entry *msg_table)
{
  funcEntry(logF,ip_Address,"clear_temp_entry_table");
  int i=0;
  printToLog(logF,ip_Address,"clear temp entry table");
  for(i=0;i<MAX_HOSTS;i++){
     memset(&msg_table[i],0,sizeof(struct hb_entry));
  }
  funcExit(logF,ip_Address,"clear_temp_entry table",0);
  return 0;
}

/*
 * This function is used to update the host's heart-beat periodically
 *
 */

int update_my_heartbeat()
{
  funcEntry(logF,ip_Address,"update_my_heart_beat");
  struct timeval timer;
  pthread_mutex_lock(&table_mutex);
  hb_table[host_no].hb_count++;
  gettimeofday(&timer,NULL);
  sprintf(hb_table[host_no].time_stamp,"%ld",timer.tv_sec);
  printToLog(logF,"UPDATE MY HB",hb_table[host_no].time_stamp);
  pthread_mutex_unlock(&table_mutex);
  funcExit(logF,ip_Address,"update_my_heart_beat",0);
  return 0;
}

/*
 * This function provides functionality to check the table for failed hosts
 * periodically.
 */

int check_table_for_failed_hosts()
{ 
  funcEntry(logF,ip_Address,"check_table_for_failed_hosts");
  int i;
  struct timeval timer;
  pthread_mutex_lock(&table_mutex);
  for(i=0;i<MAX_HOSTS;i++){
       if(hb_table[i].valid==1){
                gettimeofday(&timer,NULL);
                long int cmp_time = atoi(hb_table[i].time_stamp);
                if((timer.tv_sec - cmp_time) >= TFAIL){
                           hb_table[i].status = DOWN;
                           sprintf(logMsg1,"Entry %d is being marked DOWN at %ld time, time at the last_update is %s\n",i,timer.tv_sec,hb_table[i].time_stamp);
                           printToLog(logF,ip_Address,logMsg1);
                }
                if((timer.tv_sec - cmp_time) >= TREMOVE){
                           delete_entry_table(i);
                           sprintf(logMsg1,"Entry %d is being removed at %ld time, time at the last_update is %s\n",i,timer.tv_sec,hb_table[i].time_stamp);  
                           printToLog(logF,ip_Address,logMsg1);
                }
       }
  }
  pthread_mutex_unlock(&table_mutex);
  funcExit(logF,ip_Address,"check_table_for_failed_hosts",0);
  return 0;
}

/*
 *  This function provides functionality to convert messages to platform independent 
 *  messages. Helps convert to network to host endianness
 *  input params : message which needs to be converted 
 */

int network_to_host(char *message)
{
   int i;
   int len_str;
   int div = sizeof(int);
   len_str = strlen(message);
   int residual = len_str % div;
   int *ptr;
   ptr=(int *)message;
   for(i=0;i<=len_str/4;i++){
     *ptr=htonl(*ptr);
     ptr++;
   }
}

/*
 * This function helps convert messages from host to network endianess
 *
 */
int host_to_network(char *message)
{
   int i;
   int len_str;
   int div = sizeof(int);
   len_str=strlen(message);
   int *ptr;
   ptr=(int *)message;
   for(i=0;i<=len_str/4;i++){
     *ptr= ntohl(*ptr);
      ptr++;
   }
}

/*
 * This function initializes the required member. This is required for the leader
 * to join
 */

int initialize_table_with_member(char *port,char *ip, int host_id)
{
  int i=0;
  char portNo[100];
  char ipAddr[100];
  int hostId;
  hostId=host_id;
  strcpy(ip_Address,ip);
  strcpy(portNo, port);
  strcpy(ipAddr, ip);
  funcEntry(logF,ip_Address,"initialize_member_entry");
  printToLog(logF,ip_Address,"Initializing gossip table");

  for(i=0;i<MAX_HOSTS;i++){
      if(i==host_id){
         hb_table[i].valid=1;   // initialize the appropriate the host_id entry
         struct timeval start;
         gettimeofday(&start,NULL);
         char buffer[70];
         sprintf(buffer,"%d_%ld",hostId,start.tv_sec);

         strcpy(hb_table[i].host_id,buffer); // initialize host_id
         strcpy(hb_table[i].IP,ipAddr); // initialize ip
         printToLog(logF, "I HOPE THIS IS NOT TRUNCATED", hb_table[i].IP);
         strcpy(hb_table[i].port,portNo);  // initialize port
         hb_table[i].hb_count=1;
         strcpy(hb_table[i].time_stamp,"0");
         hb_table[i].status=1;
         update_my_heartbeat();
    }
  }
    funcExit(logF,ip_Address,"initialize_member_table",0);
return 0;

}

 

/*
 * This function helps to initialize the table with appropriate values
 * input params : port, ip and the host_id
 */

int initialize_table(char *port,char *ip,int host_id)
{
  int i=0;
  char portNo[100];
  char ipAddr[100];
  int hostId;
  hostId=host_id;
  strcpy(ip_Address,ip);
  strcpy(portNo, port);
  strcpy(ipAddr, ip);
  host_no = host_id;
  funcEntry(logF,ip_Address,"initialize_table");
  printToLog(logF,ip_Address,"Initializing gossip table");
  printToLog(logF,"DEBUG ME",port);
  printToLog(logF,"DEBUg ME",ip);
  printToLog(logF,"DEBUG ME",portNo);
  printToLog(logF,"DEBUg ME",ipAddr);
  for(i=0;i<MAX_HOSTS;i++)
    {
      if(i!=host_id){
         memset(&hb_table[i],0,sizeof(struct hb_entry));
         hb_table[i].hb_count=-1;}
      else {
         hb_table[i].valid=1;   // initialize the appropriate the host_id entry
         struct timeval start;
         gettimeofday(&start,NULL);
         char buffer[70];
         sprintf(buffer,"%d_%ld",hostId,start.tv_sec);

         strcpy(hb_table[i].host_id,buffer); // initialize host_id
         strcpy(hb_table[i].IP,ipAddr); // initialize ip
         printToLog(logF, "I HOPE THI IS NOT TRUNCATED", hb_table[i].IP);
         strcpy(hb_table[i].port,portNo);  // initialize port
         hb_table[i].hb_count=1;
         strcpy(hb_table[i].time_stamp,"0");
         hb_table[i].status=1;
         update_my_heartbeat();
      }
    }
    funcExit(logF,ip_Address,"initialize_table",0);

return 0;
}    

/*
 * This function provides functionality to serialize the table objects to a message
 * input params : buffer to which the serialized objects will be populated.
 *
 */
int create_message(char *buffer)
{
   funcEntry(logF,ip_Address,"create_message");
   int i;
   char msg[200];
   pthread_mutex_lock(&table_mutex);
   for(i=0;i<MAX_HOSTS;i++){
       sprintf(msg,"%d:%s:%s:%s:%d:%s:%d;",hb_table[i].valid,hb_table[i].host_id,hb_table[i].IP,hb_table[i].port,hb_table[i].hb_count,hb_table[i].time_stamp,hb_table[i].status);
       strcat(buffer,msg);
       memset(msg,0,200);
   }     
   printToLog(logF,ip_Address,msg);
   pthread_mutex_unlock(&table_mutex);
   funcExit(logF,ip_Address,"create_message",0);
   return 0;
}

/*
 * This function provides functionality to print the table
 */

int print_table(struct hb_entry *table)
{
   int i=0;
   
   printf("\n valid\t::\thost_id\t\t::\tIP\t\t::\tPORT\t::\tHB_COUNT\t::\tTIME STAMP\t::\tSTATUS\n");
   pthread_mutex_lock(&table_mutex);
   for(i=0;i<MAX_HOSTS;i++){
   if(table[i].valid){ 
   printf("%d\t::\t%s\t::\t%s\t::\t%s\t::\t%d\t\t::\t%s\t\t::\t%d\n",table[i].valid,table[i].host_id,table[i].IP,table[i].port,table[i].hb_count,table[i].time_stamp,table[i].status);
    }
   }
   pthread_mutex_unlock(&table_mutex);
   return 0;
}

/*
 * This interface provides functionality to update the table comparing the time_stamps
 * in the received message objects
 */

int update_table(struct hb_entry *msg_table)
{ 
  int i=0;
  funcEntry(logF,ip_Address,"update_table");
  pthread_mutex_lock(&table_mutex);
  for(i=0;i<MAX_HOSTS;i++){
       if(msg_table[i].valid && msg_table[i].status){
              if(msg_table[i].hb_count > hb_table[i].hb_count){
                       if(!hb_table[i].valid){
                                 hb_table[i].valid=1;
                                 strcpy(hb_table[i].host_id,msg_table[i].host_id);
                                 strcpy(hb_table[i].IP,msg_table[i].IP); 
                                 strcpy(hb_table[i].port,msg_table[i].port);
                                 hb_table[i].hb_count=msg_table[i].hb_count;
                                 struct timeval cur_t;
                                 gettimeofday(&cur_t,NULL);
                                 char buffer[50];
                                 sprintf(buffer,"%ld",cur_t.tv_sec);
                                 strcpy(hb_table[i].time_stamp,buffer);
                                 hb_table[i].status=msg_table[i].status;
                        }
                       else {       
                      	 hb_table[i].hb_count=msg_table[i].hb_count;
                      	 struct timeval cur_t;
                     	 gettimeofday(&cur_t,NULL);
                       	 char buffer[50];
                         sprintf(buffer,"%ld",cur_t.tv_sec);
                         strcpy(hb_table[i].time_stamp,buffer);
                         hb_table[i].status=msg_table[i].status;
                       }
                       sprintf(logMsg, "%d\t::\t%s\t::\t%s\t::\t%s\t::\t%d\t\t::\t%s\t\t::\t%d\n",hb_table[i].valid,hb_table[i].host_id,hb_table[i].IP,hb_table[i].port,hb_table[i].hb_count,hb_table[i].time_stamp,hb_table[i].status);
              }
       }
  }
  clear_temp_entry_table(msg_table);
  pthread_mutex_unlock(&table_mutex);
  printToLog(logF,ip_Address,logMsg1);
  funcExit(logF,ip_Address,"update_table",0);
  return 0;
}


/*
 * This function provides functionality to deserialize the message received
 * input params : received message
 */

struct hb_entry* extract_message(char *input)
{ 
   funcEntry(logF,ip_Address,"extract_message");
   int j=0; 
   char a[4][100];
   char *main_entry=input;
   char delim[2]=":";
   int  no_of_entries = 4;
   int  i=0;
   char *ptr; 
   ptr=strtok(main_entry,";");
   while (ptr != NULL)
   {
            strcpy(a[j], ptr);
            ptr = strtok(NULL, ";");
            j++;
    }
    i=0;
    while(i<j && i<MAX_HOSTS){
                                  
               char *token=strtok(a[i],delim);
               entry[i].valid=atoi(token);

	       if(entry[i].valid){    
     	 
        	    token=strtok(NULL,delim);
           	    strcpy(entry[i].host_id,token);
 
                    token=strtok(NULL,delim);
                    strcpy(entry[i].IP,token);
  
                    token=strtok(NULL,delim);
                    strcpy(entry[i].port,token);
 
                    token=strtok(NULL,delim);
                    entry[i].hb_count = atoi(token);
            
                    token=strtok(NULL,delim);
                    strcpy(entry[i].time_stamp,token);
        
                    token=strtok(NULL,delim);
                    entry[i].status = atoi(token);
               }
              else{
                 memset(&entry[i],0,sizeof(struct hb_entry));
              }
       i++;
    }
    funcExit(logF,ip_Address,"extract_message",0);
    return entry;            
      
}

/* initialize 2 hosts */
void initialize_two_hosts(struct two_hosts* ptr)
{
   memset(&ptr[0],0,sizeof(struct two_hosts));
   memset(&ptr[1],0,sizeof(struct two_hosts));
}

/* algorithm for finding n neighboring hosts */

int choose_n_hosts(struct two_hosts *ptr, int choice)
{
  funcEntry(logF,ip_Address,"choose_n_hosts");
  int i;
  int list[MAX_HOSTS];
  int k=0;
  int vis[MAX_HOSTS];
  for(i=0;i<MAX_HOSTS;i++)vis[i]=0;
  for(i=0;i<MAX_HOSTS;i++){

          if(i!=host_no && hb_table[i].valid && hb_table[i].status){
  
               printToLog(logF,"INSIDE IF IN CHOSEN HOSTS","FIRST IF");
               list[k]=i;
               k++;
           }
  }
  if (k==0){
      funcExit(logF, "", "choose_n_hosts", -1);
      return -1;
  }
  if(k==1){
     ptr[0].host_id = list[0];
     ptr[0].valid = 1;
     funcExit(logF, "", "choose_n_hosts", 1);
     return k;
  }
  if(k>1){
      int value;
      int m=0;
          while(1){
               value = rand()%k;
               if(vis[value] == 0){
                    vis[value]=1;
                    ptr[m].host_id=list[value];
                    ptr[m].valid=1;
                    m++;
               }
             if(m==choice)
               break;
          }
  }             
 funcExit(logF,ip_Address,"choose_n_hosts",2);
 return choice;
}
  

/*
 * This functionality is deprecated and not used anymore by daisy
 *
 */

void go_live(char *message)
{
  char *buffer = (char *)malloc(2000);
  sprintf(buffer,"%d#%s",JOIN_OPCODE,message);  
}
       

/*
void main()
{ 
*/

/*


char *ptr=(char *)malloc(200);
strcpy(ptr,"1:0_1380475981:192.168.100.120:1234:123:0:1;1:1_1234567891:192.123.456.678:123:12345:0:0;0::::0::0;0::::0::0;");
log1=fopen("./log.txt","w");
struct hb_entry *hb_entry1=extract_message(ptr);
print_table(hb_entry1);

//char buffer[200]="KARTHIK";
//sprintf(buffer,"%d::%s",JOIN_OPCODE,"karthik");
//printf("hellooooooooooooooooooo  %s helooooooooooo",buffer);
//update_table(hb_entry1);

//printf("\n%s\n",hb_entry1[0].IP);
//printf("%d\n",hb_entry1[0].valid);
initialize_table("1234","192.168.100.100",0);
//printf("%s\n",hb_table[0].IP);
//printf("%s\n",hb_table[0].host_id);

//char *buffer;
//printf("i am here\n");
//buffer=create_message();
//printf("%s",buffer);
char buffer[500];
memset(buffer,0,500);
update_table(hb_entry1);
//while(1)
{
sleep(1);
//char *buffer = (char *)malloc(300);
//update_my_heartbeat();
check_table_for_failed_hosts();
create_message(buffer);
printf("\nbefore conversion %s\n",buffer);

network_to_host(buffer);
printf("\nnetwork to host : %s\n",buffer);
host_to_network(buffer);
printf("\nhost to network : %s\n",buffer);
print_table(hb_table);
check_table_for_failed_hosts();
printf("\n%s\n",buffer);
memset(buffer,0,500);
//free(buffer);
}
//char *buffer = (char *)malloc(300);
//create_message(buffer);
//printf("\n%s\n",buffer);
}*/
