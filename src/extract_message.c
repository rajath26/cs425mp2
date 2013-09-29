#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define UP 1
#define DOWN 0

struct hb_entry {
  char host_id[70];  // combination of host_id and time_of_creation
  char IP[80];
  char port[10];
  int hb_count;
  char time_stamp[20];
  int status;
}; 
struct hb_entry entry[4];
struct hb_entry hb_table[4];


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
    i=0;
    while(i<j){
           
            char *token=strtok(a[i],delim);
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
            i++;
    }
    return entry;            
      
}
       

void main()
{
char *ptr=(char *)malloc(200);
strcpy(ptr,"1234:192.168.1.1:1234:34567:45678:1");
struct hb_entry *hb_entry1=extract_message(ptr);
printf("%s",hb_entry1[0].IP);
}
