#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include"logger.c"
#include"message.h"
#define TREMOVE 20

//FILE *log=fopen("log.txt","w");
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

FILE *log1;

int delete_entry_table(int table_index)
{
   memset(&hb_table[table_index],0,sizeof(struct hb_entry));
   char message[100];
   sprintf(message,"host_entry %d in the hb_table is deleted",table_index);
   printToLog(log1,hb_table[host_no].host_id,message);
   return 0;
}

int clear_temp_entry_table(struct hb_entry *msg_table)
{
  int i=0;
  char message[100];
 // printToLog(log1,hb_table[host_no].host_id,"temporary entry is being deleted");
  fprintf(log1,message,"%s","deleting your entry");
  for(i=0;i<MAX_HOSTS;i++){
     memset(&msg_table[i],0,sizeof(struct hb_entry));
  }
  return 0;
}

int update_my_heartbeat()
{
  struct timeval timer;
  char buffer[32];
  pthread_mutex_lock(&table_mutex);
  hb_table[host_no].hb_count++;
  gettimeofday(&timer,NULL);
  sprintf(hb_table[host_no].time_stamp,"%ld",timer.tv_sec);
  pthread_mutex_unlock(&table_mutex);
  return 0;
}

int check_table_for_failed_hosts()
{
  int i;
  struct timeval timer;
  pthread_mutex_lock(&table_mutex);
  for(i=0;i<MAX_HOSTS;i++){
       if(hb_table[i].valid==1){
                gettimeofday(&timer,NULL);
                long int cmp_time = atoi(hb_table[i].time_stamp);
                if((timer.tv_sec - cmp_time) >= TFAIL){
                           hb_table[i].status = DOWN;
                           /*put a logger message*/
                           char message[100];
                           sprintf(message,"Entry %d is being marked DOWN\n",i);
                           printToLog(log1,hb_table[host_no].host_id,message);
                }
                if((timer.tv_sec - cmp_time) >= TREMOVE){
                           delete_entry_table(i);
                           char message[100];
                           sprintf(message,"Entry %d is being removed\n",i);  
                           printToLog(log1,hb_table[host_no].host_id,message);
                }
       }
  }
  pthread_mutex_unlock(&table_mutex);
  return 0;
}

/*
void periodic_heartbeat_update()
{
  pthread_mutex_lock(&table_mutex);
  while(1){
       sleep(HEART_BEAT_UPDATE_SEC);
       update_my_entry();
       check_table_for_failed_hosts();
  }
  pthread_mutex_unlock(&table_mutex);
}        
*/  

//network to host and host to network functionality added
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

int initialize_table()
{
  int i=0;
  printToLog(log1,hb_table[host_no].host_id,"Initializing gossip table");
//  pthread_mutex_lock(&table_mutex);
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
         hb_table[i].hb_count=-1;
         strcpy(hb_table[i].time_stamp,"0");
         hb_table[i].status=1;
         update_my_heartbeat();
      }
    }
//  pthread_mutex_unlock(&table_mutex);

return 0;
}    
int create_message(char *buffer)
{
   int i;
   char msg[200];
   pthread_mutex_lock(&table_mutex);
   for(i=0;i<MAX_HOSTS;i++){
       sprintf(msg,"%d:%s:%s:%s:%d:%s:%d;",hb_table[i].valid,hb_table[i].host_id,hb_table[i].IP,hb_table[i].port,hb_table[i].hb_count,hb_table[i].time_stamp,hb_table[i].status);
       strcat(buffer,msg);
       memset(msg,0,200);
   }     
   pthread_mutex_unlock(&table_mutex);
   return 0;
}

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

int update_table(struct hb_entry *msg_table)
{ 
  int i=0;
  pthread_mutex_lock(&table_mutex);
  clear_temp_entry_table(msg_table);
  for(i=0;i<MAX_HOSTS;i++){
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
  pthread_mutex_unlock(&table_mutex);
  return 0;
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

int choose_n_hosts(struct two_hosts *ptr, int choice)
{
  int i=0;
  int k=0;
  int count=0;
  int value;
  int list[MAX_HOSTS];
  for(i=0;i<MAX_HOSTS;i++) list[i]=0;
  int counter=0;
  for(i=0;i<MAX_HOSTS;i++){
         if(hb_table[i].valid){
                     list[counter]=i;
                     counter++;
         }
  }
  if(counter==1)
    return -1;
  // choose the remaining host
  if(counter==2){
       for(i=0;i<counter;i++){
              if(i!=host_no){
                    ptr[k].host_id=list[i];
                    ptr[k].valid = 1;
               }       
       }
  }
  //general case - iterate till you find choice number of hosts
  for(i=0;i<choice;i++){
        while(1){ // don't find me
              value=rand()%counter;
              if(list[value]!=host_no)
              break;
        }
      ptr[k].host_id=list[value];
      ptr[k].valid=1;
      k++;
  } 
}
  
/*
int choose_two_hosts(struct two_hosts* ptr)
{
  int cur_host = host_no;
  int nbr_host = (host_no+1)%MAX_HOSTS;
  int i=0;
  int count=0;
  pthread_mutex_lock(&table_mutex);
  for(i=0;i<MAX_HOSTS-1;i++){
       if(hb_table[nbr_host].valid){
                 ptr[count].host_id = nbr_host;
                 ptr[count].valid = 1;
                 count++;
        if(count==2) break;
       }
       nbr_host=(nbr_host+1)%MAX_HOSTS; 
  }
  pthread_mutex_unlock(&table_mutex);
 return count;             
}  
*/
void go_live(char *message)
{
  char *buffer = (char *)malloc(2000);
  sprintf(buffer,"%d#%s",JOIN_OPCODE,message);  
}
       
/*
void main()
{ */


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
initialize_table();
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
