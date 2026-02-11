#ifndef PACKET_H
#define PACKET_H


#define MAX_SIZE 16


#include <stdint.h>

typedef struct {
    uint8_t seq_no;     // 1 byte
    uint8_t pad;        // 1 byte (keeps total size 4)
    uint16_t data;      // 2 bytes payload (16 bits)
} Packet;



typedef struct {
    uint8_t ack_no;
} Ack;



#endif