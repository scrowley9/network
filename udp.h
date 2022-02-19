#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <errno.h>

#define ERROR -1
#define DST_UDP_PORT 6000 // UDP Server Port
#define MAXLINE 1024

int create_broadcast_socket(char* ip, struct sockaddr_ll* endpoint);
void udp_send_message(char* buff, size_t size, int udp_socket, struct sockaddr* endpoint);
void udp_cleanup(int udp_socket);
// void udp_recv_message(int udp_socket, struct sockaddr* endpoint);
