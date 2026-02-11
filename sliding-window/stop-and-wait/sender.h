#ifndef SENDER_H
#define SENDER_H


#include "packet.h"


void sender_init(int socket_fd);
void sender_send(Packet *pkt);
void sender_on_ack(Ack ack);
void sender_on_timeout();


#endif