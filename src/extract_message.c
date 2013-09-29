#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define UP 1
#define DOWN 0
#define MAX_HOSTS 4

char *host_ip_address="192.168.100.100";
char *host_port="1234";

int host_no=0;

struct hb_entry {
  int  valid;
  char host_id[70];  // combination of host_id and time_of_creation
  char IP[80];
  char port[10];
  int hb_count;
  char time_stamp[20];
  int status;
}; 

struct hb_entry entry[4];
struct hb_entry hb_table[4];

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
       i++;
    }
    return entry;            
      
}
       

void main()
{
char *ptr=(char *)malloc(200);
strcpy(ptr,"1:0_1380475981:192.168.100.120:1234:0:0:1;0::::0::0;0::::0::0;0::::0::0;");

struct hb_entry *hb_entry1=extract_message(ptr);

printf("\n%s\n",hb_entry1[0].IP);
printf("%d\n",hb_entry1[0].valid);

initialize_table();
printf("%s\n",hb_table[0].IP);
printf("%s\n",hb_table[0].host_id);

char *buffer;
printf("i am here\n");
buffer=create_message();
printf("%s",buffer);
print_table(hb_table);
print_table(entry);
}
