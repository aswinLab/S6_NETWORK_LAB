#ifndef PACKET_H
#define PACKET_H


#define MAX_SIZE 16


typedef struct {
    int seq_no;
    int data[MAX_SIZE];
} Packet;


typedef struct {
    int ack_no;
} Ack;


#endif