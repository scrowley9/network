#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define ERROR -1
#define DST_UDP_PORT 6000 // UDP Server Port
#define MAXLINE 1024

int create_broadcast_socket(int i, char* ip, struct sockaddr_in* endpoint);
void udp_send_message(char* buff, size_t size, int udp_socket, struct sockaddr* endpoint);
void udp_cleanup(int udp_socket);
// void udp_recv_message(int udp_socket, struct sockaddr* endpoint);
