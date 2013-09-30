#define UP 1
#define DOWN 0
#define MAX_HOSTS 4
#define TFAIL 10
#define JOIN_OPCODE 1
#define LEAVE_OPCODE 2
#define MAX_CHOSEN_HOSTS 2

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
