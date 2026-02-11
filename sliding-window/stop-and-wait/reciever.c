#include "reciever.h"
#include "protocol.h"
#include <sys/socket.h>
#include <stdio.h>


static int sock;
static int expected_seq = 0;


void receiver_init(int socket_fd) {
    sock = socket_fd;
}


void receiver_on_packet(Packet pkt) {
    printf("Recieved packet%d\n", pkt.seq_no);
    Ack ack;


    if (pkt.seq_no == expected_seq) {
        // deliver data upward
        ack.ack_no = pkt.seq_no;
        expected_seq = (expected_seq + 1) % SEQ_MODULO;
    }
    else{
        expected_seq = (expected_seq + SEQ_MODULO -1) % SEQ_MODULO;
    }


    ack.ack_no = (expected_seq + 1) % SEQ_MODULO;
    send(sock, &ack, sizeof(Ack), 0);
}