#include<pthread.h>

#define UP 1
#define DOWN 0
#define MAX_HOSTS 4
#define TFAIL 10
#define JOIN_OPCODE 1
#define LEAVE_OPCODE 2
#define MAX_CHOSEN_HOSTS 2
#define HOSTID_LENGTH 70
#define IP_ADDRESS_LEN 30
#define PORT_LENGTH 5
#define TIME_STAMP_LEN 20
#define HEART_BEAT_UPDATE_SEC 1

char *host_ip_address="192.168.100.100";
char *host_port="1234";

pthread_mutex_t table_mutex;

int host_no=0;

struct two_hosts{
  int host_id;
  int valid;
};

struct hb_entry {
  int  valid;
  char host_id[HOSTID_LENGTH];  // combination of host_id and time_of_creation
  char IP[IP_ADDRESS_LEN];
  char port[PORT_LENGTH];
  int hb_count;
  char time_stamp[TIME_STAMP_LEN];
  int status;
};

struct hb_entry entry[MAX_HOSTS];  // this table is used to extract values from the message
struct hb_entry hb_table[MAX_HOSTS];  // this is the heart beat table mantained for a single host

void update_my_entry();
void check_table_for_failed_hosts();
void periodic_heartbeat_update();
void initialize_table();
char *create_message();
void print_table(struct hb_entry*);
void update_table(struct hb_entry *);
struct hb_entry* extract_message(char *);
void initialize_two_hosts(struct two_hosts *);
int choose_two_hosts(struct two_hosts *);
void go_live(char *);
void clear_temp_entry_table();
