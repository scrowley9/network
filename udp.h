#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define ERROR 1
#define UDP_PORT 6000 // Binding for UDP messages

void udp_send_message(uint8_t* buff, size_t size, char* ip);
