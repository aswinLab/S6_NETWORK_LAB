#include "sender.h"
#include "protocol.h"


static int sock;
static int next_seq = 0;
static int waiting = 0;
static Packet current;


void sender_init(int socket_fd) {
sock = socket_fd;
}


void sender_send(int *data) {
if (waiting) return;


current.seq_no = next_seq;
for (int i = 0; i < MAX_SIZE; i++)
current.data[i] = data[i];


send(sock, &current, sizeof(Packet), 0);
waiting = 1;
}


void sender_on_ack(Ack ack) {
if (ack.ack_no == next_seq) {
waiting = 0;
next_seq = (next_seq + 1) % SEQ_MODULO;
}
}


void sender_on_timeout() {
send(sock, &current, sizeof(Packet), 0);
}