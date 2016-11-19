/* CSCI4061 F2016 Assignment 3
* login: engel429
* date: 11/19/2016
* section: 7
* name: Joey Engelhart, Dawei Luo, Jerry Nguyen (for partner(s))
* id: engel429, luoxx417, nguy1544 */ 

#include "packet.h"

int msqid = -1;

static message_t message;   /* current message structure */
static mm_t mm;             /* memory manager will allocate memory for packets */
static int pkt_cnt = 0;     /* how many packets have arrived for current message */
static int pkt_total = 1;   /* how many packets to be received for the message */

/*
   Handles the incoming packet. 
   Store the packet in a chunk from memory manager.
   The packets for given message will come out of order. 
   Hence you need to take care to store and assemble it correctly.
   Example, message "aaabbb" can come as bbb -> aaa, hence, you need to assemble it
   as aaabbb.
   Hint: "which" field in the packet will be useful.
 */
static void packet_handler(int sig) {
  packet_t pkt;
  void *chunk;
  packet_queue_msg pqm;    
  // get the "packet_queue_msg" from the queue.
  if(msgrcv(msqid, &pqm, sizeof(packet_t), QUEUE_MSG_TYPE, 0) == -1) {
      perror("Failed to receive packet from message queue: exiting\n");
      if (msgctl(msqid, IPC_RMID, NULL) == -1) {
          perror("Failed to destroy msgq!\n");
      }
      exit(0);
  }
  // extract the packet from "packet_queue_msg" and store it in the memory from memory manager
  pkt = pqm.pkt;
  void* mem_block = mm_get(&mm);
  memcpy(mem_block, &(pkt.data), sizeof(data_t));

  message.data[pkt.which] = mem_block;
  message.num_packets++;
  // once this has received all packets, pkt_cnt is used to break from pause() loop in main()
  if (message.num_packets == pkt.how_many) {
      pkt_cnt = 1;
  }
}

/*
 * Create message from packets ... deallocate packets.
 * Return a pointer to the message on success, or NULL
 */
static char *assemble_message() {

  char *msg;
  int i;
  int msg_len = message.num_packets * sizeof(data_t);

  /* Allocate msg and assemble packets into it */

  if((msg = (char *) malloc(msg_len + 1)) == NULL) {
      perror("Failed to allocate space for assemble_message: exiting\n");
      if (msgctl(msqid, IPC_RMID, NULL) == -1) {
          perror("Failed to destroy msgq!\n");
      }
      exit(0);
  }
  /* assemble message in order indicated by message.data (pkt pointers stored in message.data at index = which) */
  int pktsize = sizeof(data_t);
  for(i = 0; i < message.num_packets; i++) {
      memcpy(msg + (i * pktsize), (message.data)[i], pktsize);
  }
  *(msg + msg_len) = '\0';

  /* reset these for next message */
  pkt_total = 1;
  pkt_cnt = 0;
  message.num_packets = 0;

  return msg;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: packet_sender <num of messages to receive>\n");
    exit(-1);
  }

  int k = atoi(argv[1]); /* no of messages you will get from the sender */
  int i;
  char *msg;
  struct sigaction act;

  /* init memory manager for NUM_CHUNKS chunks of size CHUNK_SIZE each */

  mm_init(&mm, NUM_CHUNKS, CHUNK_SIZE);
  message.num_packets = 0;

  /* initialize msqid to send pid and receive messages from the message queue. Use the key in packet.h */
  
  msqid = msgget(key, PERMS);

  /* send process pid to the sender on the queue */

  pid_queue_msg *pidqm;
  if((pidqm = (pid_queue_msg *) malloc(sizeof(pid_queue_msg))) == NULL) {
      perror("Failed to allocate pid_queue_msg for packet_receiver to put in queue: aborting\n");
      if (msgctl(msqid, IPC_RMID, NULL) == -1) {
          perror("Failed to destroy msgq!\n");
      }
      exit(0);
  }
  pidqm->mtype = QUEUE_MSG_TYPE;
  pidqm->pid = getpid();

  if(msgsnd(msqid, pidqm, sizeof(int), 0) == -1) {
      perror("Failed to send receiver pid into message queue: aborting\n");
      if (msgctl(msqid, IPC_RMID, NULL) == -1) {
          perror("Failed to destroy msgq!\n");
      }
      exit(0);
  }
  
  /* set up SIGIO handler to read incoming packets from the queue. Check packet_handler()*/

  act.sa_handler = packet_handler;
  act.sa_flags = 0;
  if( (sigfillset(&act.sa_mask) == -1) ||
      (sigaction(SIGIO, &act, NULL) == -1) ) {
      
      perror("Failed to set SIGIO signal handler: exiting\n");
      exit(0);
  }

  for (i = 1; i <= k; i++) {
    while (pkt_cnt < pkt_total) {
      pause(); /* block until next packet */
    }
  
    msg = assemble_message();
    if (msg == NULL) {
      perror("Failed to assemble message");
      if (msgctl(msqid, IPC_RMID, NULL) == -1) {
          perror("Failed to destroy msgq!\n");
      }
    }
    else {
      fprintf(stderr, "GOT IT: message=%s\n", msg);
//      free(msg);
    }
  }

  // deallocate memory manager

  mm_release(&mm);

  // remove the queue once done
  if(msgctl(msqid, IPC_RMID, NULL) == -1) {
    perror("Failed to destroy message queue: exiting\n");
    exit(0);
  }
  
  return EXIT_SUCCESS;
}
