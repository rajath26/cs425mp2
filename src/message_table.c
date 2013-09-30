#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include"message.h"

/*
#define UP 1
#define DOWN 0
#define MAX_HOSTS 4
#define TFAIL 10
#define JOIN_OPCODE 1
#define LEAVE_OPCODE 2

char *host_ip_address="192.168.100.100";
char *host_port="1234";

int host_no=0;

struct two_hosts{
  int host_id;
  int valid;
};

struct hb_entry {
  int  valid;
  char host_id[70];  // combination of host_id and time_of_creation
  char IP[80];
  char port[10];
  int hb_count;
  char time_stamp[20];
  int status;
}; 

struct hb_entry entry[4];  // this table is used to extract values from the message
struct hb_entry hb_table[4];  // this is the heart beat table mantained for a single host
*/

void update_my_entry()
{
  hb_table[host_no].hb_count++;
}

void check_table_for_failed_hosts()
{
  int i;
  struct timeval timer;
  for(i=0;i<MAX_HOSTS;i++){
       if(hb_table[i].valid==1){
                gettimeofday(&timer,NULL);
                long int cmp_time = atoi(hb_table[i].time_stamp);
                if((timer.tv_sec - cmp_time) >= TFAIL){
                           hb_table[i].status = DOWN;
                }
       }
  }
}
void periodic_heartbeat_update()
{
  while(1){
       sleep(1);
       update_my_entry();
       check_table_for_failed_hosts();
  }
}        
  

void initialize_table()
{
  int i=0;
  for(i=0;i<MAX_HOSTS;i++)
    {
      if(i!=host_no)
         memset(&hb_table[i],0,sizeof(struct hb_entry));
      else {
         hb_table[i].valid=1;   // initialize the appropriate the host_id entry
         struct timeval start;
         gettimeofday(&start,NULL);
         char buffer[70];
         sprintf(buffer,"%d_%ld",host_no,start.tv_sec);

         strcpy(hb_table[i].host_id,buffer); // initialize host_id
         strcpy(hb_table[i].IP,host_ip_address); // initialize ip
         strcpy(hb_table[i].port,host_port);  // initialize port
         hb_table[i].hb_count=0;
         strcpy(hb_table[i].time_stamp,"0");
         hb_table[i].status=1;
      }
    }
}    
char* create_message()
{
   int i;
   char msg[sizeof(struct hb_entry)+30];
   char *buffer=(char *)malloc((4*sizeof(struct hb_entry))+50);
   for(i=0;i<MAX_HOSTS;i++){
       sprintf(msg,"%d:%s:%s:%s:%d:%s:%d;",hb_table[i].valid,hb_table[i].host_id,hb_table[i].IP,hb_table[i].port,hb_table[i].hb_count,hb_table[i].time_stamp,hb_table[i].status);
       strcat(buffer,msg);
   }     
   return buffer;
}

void print_table(struct hb_entry *table)
{
   int i=0;
   printf("\n valid\t::\thost_id\t\t::\tIP\t\t::\tPORT\t::\tHB_COUNT\t::\tTIME STAMP\t::\tSTATUS\n");
   for(i=0;i<MAX_HOSTS;i++){
   if(table[i].valid){ 
   printf("%d\t::\t%s\t::\t%s\t::\t%s\t::\t%d\t\t::\t%s\t\t::\t%d\n",table[i].valid,table[i].host_id,table[i].IP,table[i].port,table[i].hb_count,table[i].time_stamp,table[i].status);
    }
   }
}

void update_table(struct hb_entry *msg_table)
{ 
  int i=0;
  for(i=0;i<4;i++){
       if(msg_table[i].valid){
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
              }
       }
  }
}



struct hb_entry* extract_message(char *input)
{ 
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
            printf("%s\n",ptr);         
            ptr = strtok(NULL, ";");
            j++;
    }
    printf("no_entries :%d\n",j);
    i=0;
    while(i<j){
            
               printf("inside_while : %s\n",a[i]);
                      
               char *token=strtok(a[i],delim);
               entry[i].valid=atoi(token);
               //  printf("i am here\n");
               printf("valid: %d\n",entry[i].valid);
	       if(entry[i].valid){    
     	 
        	    token=strtok(NULL,delim);
           	    strcpy(entry[i].host_id,token);
       	            printf("%s\n",entry[i].host_id);

                    token=strtok(NULL,delim);
                    strcpy(entry[i].IP,token);
                    printf("%s\n",entry[i].IP);            
  
                    token=strtok(NULL,delim);
                    strcpy(entry[i].port,token);
                    printf("%s\n",entry[i].port);
 
                    token=strtok(NULL,delim);
                    entry[i].hb_count = atoi(token);
                    printf("%d\n",entry[i].hb_count); 
            
                    token=strtok(NULL,delim);
                    strcpy(entry[i].time_stamp,token);
                    printf("%s\n",entry[i].time_stamp);
        
                    token=strtok(NULL,delim);
                    entry[i].status = atoi(token);
                    printf("%d\n",entry[i].status);
               }
              else{
                 memset(&entry[i],0,sizeof(struct hb_entry));
              }
       i++;
    }
    return entry;            
      
}

/* initialize 2 hosts */
void initialize_two_hosts(struct two_hosts* ptr)
{
   memset(&ptr[0],0,sizeof(struct two_hosts));
   memset(&ptr[1],0,sizeof(struct two_hosts));
}

/* algorithm for finding two neighboring hosts */

int choose_two_hosts(struct two_hosts* ptr)
{
  int cur_host = host_no;
  int nbr_host = (host_no+1)%MAX_HOSTS;
  int i=0;
  int count=0;
  for(i=0;i<MAX_HOSTS-1;i++){
       if(hb_table[nbr_host].valid){
                 ptr[count].host_id = nbr_host;
                 ptr[count].valid = 1;
                 count++;
        if(count==2) break;
       }
       nbr_host=(nbr_host+1)%MAX_HOSTS; 
  }
 return count;             
}  

void go_live(char *message)
{
  char *buffer = (char *)malloc(2000);
  sprintf(buffer,"%d#%s",JOIN_OPCODE,message);  
}
       

void main()
{
char *ptr=(char *)malloc(200);
strcpy(ptr,"1:0_1380475981:192.168.100.120:1234:123:0:1;1:1_1234567891:192.123.456.678:123:12345:0:0;0::::0::0;0::::0::0;");

struct hb_entry *hb_entry1=extract_message(ptr);
print_table(hb_entry1);

char buffer[200]="KARTHIK";
//sprintf(buffer,"%d::%s",JOIN_OPCODE,"karthik");
printf("hellooooooooooooooooooo  %s helooooooooooo",buffer);
//update_table(hb_entry1);

//printf("\n%s\n",hb_entry1[0].IP);
//printf("%d\n",hb_entry1[0].valid);

initialize_table();
//printf("%s\n",hb_table[0].IP);
//printf("%s\n",hb_table[0].host_id);

//char *buffer;
//printf("i am here\n");
//buffer=create_message();
//printf("%s",buffer);

update_table(hb_entry1);
print_table(hb_table);
}
