#include<stdio.h>
#include<stdlib.h>
#include"message_table.c"

/*
void *heart_beat_updater(void* params)
{
  while(1)
    {
      check_table_for_failed_hosts();
      hb_table[host_no].hb_count++;
      sleep(1);
    }
}*/


void periodic_heartbeat_update()
{
  while(1){
       sleep(HEART_BEAT_UPDATE_SEC);
       update_my_heartbeat();
       check_table_for_failed_hosts();
  }
}      



void *gossiper(void *params)
{
  int i;
  int no_entry;
  int ret;
  struct two_hosts host_table[2];
 // char *message = (char *)malloc(500);
  while(1){
      char *message = (char *)malloc(500);
      initialize_two_hosts(host_table);
      no_entry=choose_n_hosts(host_table,2);
      ret = create_message(message);
      if(!no_entry)
         goto Sleep;
      for(i=0;i<MAX_CHOSEN_HOSTS;i++){
          if(host_table[i].valid)     
             sendudp(hb_table[host_table[i].host_id].IP,hb_table[host_table[i].host_id].port,message);
      }
     free(message);
    Sleep:  sleep(2);
  }
}

void *reciever(void *params)
{
  struct hb_entry* temp;
  char *message;
  while(1){
      message = (char *) recvudp(hb_table[host_no].IP,hb_table[host_no].port);
      clear_temp_entry_table(entry);
      temp = extract_message(message);
      update_table(temp); 
  }
}
