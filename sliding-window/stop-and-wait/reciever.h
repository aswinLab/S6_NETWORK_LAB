#ifndef RECEIVER_H
#define RECEIVER_H


#include "packet.h"


void receiver_init(int socket_fd);
void receiver_on_packet(Packet pkt);


#endif