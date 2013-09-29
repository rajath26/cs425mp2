
struct hb_entry {
  char host_id[70];  // combination of host_id and time_of_creation
  char IP[80];
  char port[10];
  int hb_count;
  char time_stamp[20];
  int status;
};

