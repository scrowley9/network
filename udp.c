#include "udp.h"

int init_udp_socket_out(char* ip, struct sockaddr_in* endpoint){

    // Create Socket
    int domain = AF_INET; // connect to network
    int data_type = SOCK_DGRAM; // UDP - datagram
    int protocol = 0; // Use IP - This is the same number which appears on protocol field in the IP header of a packet.
    int fd = -1;

    if((fd = socket(domain, data_type, protocol)) < 0){
        // Failed!
        fprintf(stderr, "Error - Unable to create UDP socket!\n");
        return ERROR;
    }

    // Filling in endpoint information - https://www.gta.ufrj.br/ensino/eel878/sockets/structs.html
    memset(endpoint, 0, sizeof(struct sockaddr_in)); // clear out endpoint address
    
    // Set Endpoint
    endpoint->sin_family = AF_INET; // connect to network
    endpoint->sin_port = htons(8080); // HTTP port - doesn't really matter (I think)
    inet_aton(ip, &(endpoint->sin_addr)); // convert ip string and assign it

    return fd;
}

void udp_send_message(uint8_t* buff, size_t size, char* ip){

    struct sockaddr_in endpoint;
    int udp = init_udp_socket_out(ip, &endpoint); // create udp socket and set endpoint ip

    // Now simply send your message!
    sendto(udp, buff, size, 0, (struct sockaddr*)&endpoint, sizeof(struct sockaddr));

    // Cleaup
    close(udp);
    printf("Message Sent!\n");
}


