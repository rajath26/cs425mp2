#include<stdio.h>
#include<stdlib.h>
#include"message_table.c"

void *heart_beat_updater(void* params)
{
  while(1)
    {
      check_table_for_failed_hosts();
      hb_table[host_no].hb_count++;
      sleep(1);
    }
}

void *gossiper(void *params)
{
  int i;
  int no_entry;
  char *message;
  struct two_hosts host_table[2];
  while(1){
      initialize_two_hosts(host_table);
      no_entry=choose_two_hosts(host_table);
      message = create_message();
      if(!no_entry)
         goto Sleep;
      for(i=0;i<MAX_CHOSEN_HOSTS;i++){
          if(host_table[i].valid)     
             sendudp(hb_table[host_table[i].host_id].IP,hb_table[host_table[i].host_id].port,message);
      }
    Sleep:  sleep(2);
  }
}

void *reciever(void *params)
{
  struct hb_entry* temp;
  char *message;
  while(1){
      message = recvudp(hb_table[host_no].IP,hb_table[host_no].port);
      clear_temp_entry_table();
      temp = extract_message(message);
      update_table(temp); 
  }
}
