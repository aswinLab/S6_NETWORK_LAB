#include "sender.h"
#include "protocol.h"
#include <sys/socket.h>
#include <stdio.h>


static int sock;
static int next_seq = 0;
static int waiting = 0;
static Packet current;


void sender_init(int socket_fd) {
sock = socket_fd;
}


void sender_send(Packet *pkt) {

    if (waiting) return;

    current = *pkt;
    current.seq_no = next_seq;

    send(sock, &current, sizeof(Packet), 0);

    printf("[SENDER] Sent seq %d\n", current.seq_no);

    waiting = 1;
}



void sender_on_ack(Ack ack) {
    printf("Recieved ack %d\n", ack.ack_no);
if (ack.ack_no == next_seq) {
waiting = 0;
next_seq = (next_seq + 1) % SEQ_MODULO;
}
}


void sender_on_timeout() {
send(sock, &current, sizeof(Packet), 0);
}