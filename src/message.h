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


