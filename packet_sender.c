/* CSCI4061 F2016 Assignment 3
* login: engel429
* date: 11/19/2016
* section: 7
* name: Joey Engelhart, Dawei Luo, Jerry Nguyen (for partner(s))
* id: engel429, luoxx417, nguy1544 */

#include <time.h>
#include "packet.h"

static int pkt_cnt = 0;     /* how many packets have been sent for current message */
static int pkt_total = 1;   /* how many packets to send send for the message */
static int msqid = -1; /* id of the message queue */
static int receiver_pid; /* pid of the receiver */

/*
   Returns the packet for the current message. The packet is selected randomly.
   The number of packets for the current message are decided randomly.
   Each packet has a how_many field which denotes the number of packets in the current message.
   Each packet is string of 3 characters. All 3 characters for given packet are the same.
   For example, the message with 3 packets will be aaabbbccc. But these packets will be sent out order.
   So, message aaabbbccc can be sent as bbb -> aaa -> ccc
   */
static packet_t get_packet() {
  static int which = -1;
  static int how_many;
  static int num_of_packets_sent = 0;
  static int is_packet_sent[MAX_PACKETS];
  int i;

  packet_t pkt;

  // this section initializes how_many if no packets have been sent (i.e., we're at beginning)
  if (num_of_packets_sent == 0) {
    how_many = rand() % MAX_PACKETS;
    if (how_many == 0) {
      how_many = 1;
    }
    printf("Number of packets in current message: %d\n", how_many);
    which = -1;
    for (i = 0; i < MAX_PACKETS; ++i) {
      is_packet_sent[i] = 0;
    }
  } // end how_many init
  
  which = rand() % how_many;
  if (is_packet_sent[which] == 1) {
    i = (which + 1) % how_many;
    while (i != which) {
      if (is_packet_sent[i] == 0) {
        which = i;
        break;
      }
      i = (i + 1) % how_many;
    } 

  }
  pkt.how_many = how_many;
  pkt_total = how_many;
  pkt.which = which;

  memset(pkt.data, 'a' + which, sizeof(data_t));

  is_packet_sent[which] = 1;
  num_of_packets_sent++;
  if (num_of_packets_sent == how_many) {
    num_of_packets_sent = 0;
  }

  return pkt;
}

static void packet_sender(int sig) {
  packet_t pkt;

  pkt = get_packet();
  // temp is just used for temporarily printing the packet.
  char temp[PACKET_SIZE + 2];
  strcpy(temp, pkt.data);
  temp[3] = '\0';
  printf ("Sending packet: %s\n", temp);
  pkt_cnt++;

  // Create a packet_queue_msg for the current packet.
  packet_queue_msg *pqm;
  if((pqm = (packet_queue_msg *) malloc(sizeof(packet_queue_msg))) == NULL) {
      perror("Failed to allocate packet_queue_msg for packet_sender: aborting\n");
      exit(0);
  }
  pqm->pkt = pkt;
  pqm->mtype = QUEUE_MSG_TYPE;

  // send this packet_queue_msg to the receiver. Handle any error appropriately.

  if(msgsnd(msqid, pqm, sizeof(packet_t), 0) == -1) {
      perror("Failed to send packet to message queue: exiting\n");
      if (msgctl(msqid, IPC_RMID, NULL) == -1) {
          perror("Failed to destroy msgq!\n");
      }
      exit(0);
  }
  // send SIGIO to the receiver if message sending was successful.
  if(kill(receiver_pid, SIGIO) == -1) {
      perror("Failed to signal receiver process with SIGIO using kill(): aborting\n");
      if (msgctl(msqid, IPC_RMID, NULL) == -1) {
          perror("Failed to destroy msgq!\n");
      }
      exit(0);
  }
}

int main(int argc, char **argv) {

  if (argc != 2) {
    printf("Usage: packet_sender <num of messages to send>\n");
    exit(-1);
  }

  int k = atoi(argv[1]); /* number of messages  to send */
  srand(time(NULL)); /* seed for random number generator */

  int i;

  struct itimerval interval;
  struct sigaction act;           

  /* Create a message queue */ 

  if((msqid = msgget(key, IPC_CREAT | 0600)) == -1) {
      perror("Failed to create message queue: aborting\n");
      exit(0);
  }
 
  /* read the receiver pid from the queue and store it for future use*/

  pid_queue_msg r_pid;
  printf("Waiting for receiver pid.\n");
  if(msgrcv(msqid, &r_pid, sizeof(int), QUEUE_MSG_TYPE, 0) == -1) {
      perror("Failed to get receiver pid from message queue: exiting\n");
      if (msgctl(msqid, IPC_RMID, NULL) == -1) {
          perror("Failed to destroy msgq!\n");
      }
      exit(0);
  }
  receiver_pid = r_pid.pid;   
  printf("Got pid : %d\n", receiver_pid);
 
  /* set up alarm handler -- mask all signals within it */
  /* The alarm handler will get the packet and send the packet to the receiver. Check packet_sender();
   * Don't care about the old mask, and SIGALRM will be blocked for us anyway,
   * but we want to make sure act is properly initialized.
   */

  act.sa_handler = packet_sender;
  act.sa_flags = 0;
  if( (sigfillset(&act.sa_mask) == -1) ||
      (sigaction(SIGALRM, &act, NULL) == -1) ) {
   
      perror("Failed to set SIGALRM signal handler: exiting\n");
      exit(0);
  } 

  /*  
   * turn on alarm timer ...
   * use  INTERVAL and INTERVAL_USEC for sec and usec values
  */

  interval.it_interval.tv_sec = INTERVAL;
  interval.it_interval.tv_usec = INTERVAL_USEC;
  interval.it_value.tv_sec = INTERVAL;
  interval.it_value.tv_usec = INTERVAL_USEC;

  /* And the timer */

  setitimer(ITIMER_REAL, &interval, NULL);

  /* NOTE: the below code wont run now as you have not set the SIGALARM handler. Hence, 
     set up the SIGALARM handler and the timer first. */
  for (i = 1; i <= k; i++) {
    printf("==========================%d\n", i);
    printf("Sending Message: %d\n", i);
    while (pkt_cnt < pkt_total) {
      pause(); /* block until next packet is sent. SIGALARM will unblock and call the handler.*/
    }
    pkt_cnt = 0;
  }

  return EXIT_SUCCESS;
}
