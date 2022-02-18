#include "udp.h"

int create_broadcast_socket(int i, char* ip, struct sockaddr_in* endpoint){

    // AF_INET forces IPv4 and TCP headers
    // SOCK_DGRAM forces the kernel to insert its own ethernet header

    // Create Socket
    // int domain = AF_INET; // send over network using IPv4 TCP protocol
    int domain = 30; // AF_PACKET replacement -- send packet as is without transport protocol (i.e. no TCP header)
    int data_type = SOCK_RAW; // UDP socket -- don't use DGRAM because we want to fill in the ethernet header
    int protocol = htons(0x0806); // ARP protocol
    int fd = -1;

    if((fd = socket(domain, data_type, protocol)) < 0){
        // Failed!
        // MUST BE ROOT to create raw
        fprintf(stderr, "Error - Unable to create raw broadcast socket: %d\n", fd);
        return ERROR;
    }

    // Filling in endpoint information - https://www.gta.ufrj.br/ensino/eel878/sockets/structs.html
    memset(endpoint, 0, sizeof(struct sockaddr_in)); // clear out endpoint address
    
    // Set Endpoint
    endpoint->sin_family = AF_INET; // connect to network
    // endpoint->sin_port = htons(DST_UDP_PORT); // Shouldn't need a port
    // endpoint->sin_addr.s_addr = INADDR_BROADCAST; // specify broadcast address (255.255.255.255)
    inet_aton(ip, &endpoint->sin_addr); // Set destination ip
    
    // Enable broadcasting
    // int broadcastEnable=1;
    // int ret=setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

    return fd;
}

void udp_send_message(char* buff, size_t size, int udp_socket, struct sockaddr* endpoint){

    // Now simply send your message!
    ssize_t err = 0;
    if((err = sendto(udp_socket, buff, size, 0, endpoint, sizeof(struct sockaddr))) < 0){
        fprintf(stderr, "Error - Unable to send UDP packet - Error Code: %zd\n", err);
    }

    printf("Message Sent!\n");
}


void udp_cleanup(int udp_socket){
    // Cleaup
    close(udp_socket);
}