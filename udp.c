#include "udp.h"

/**
 * @brief Create a broadcast socket fd
 * REQUIRES: user to be root
 * 
 * @param ip 
 * @param endpoint 
 * @return int 
 */
int create_broadcast_socket(char* ip, struct sockaddr_ll* endpoint){

    // AF_INET forces IPv4 and TCP headers
    // SOCK_DGRAM forces the kernel to insert its own ethernet header

    // Create Socket
    // int domain = AF_INET; // send over network using IPv4 TCP protocol
    int domain = AF_PACKET; // AF_PACKET replacement (Linux only) -- send packet as is without transport protocol (i.e. no TCP header)
    int data_type = SOCK_RAW; // UDP socket -- don't use DGRAM because we want to fill in the ethernet header
    int protocol = htons(ETH_P_ARP); // ARP protocol
    int fd = -2;

    if((fd = socket(domain, data_type, protocol)) < 0){
        // Failed!
        // MUST BE ROOT to create raw
        fprintf(stderr, "Error - Unable to create raw broadcast socket: %d\n", fd);
        fprintf(stderr, "Errno: %d\n", errno);
        perror("Errno");
        return ERROR;
    }

    // Filling in endpoint information - https://www.gta.ufrj.br/ensino/eel878/sockets/structs.html
    memset(endpoint, 0, sizeof(struct sockaddr_ll)); // clear out endpoint address
    

    // Set Endpoint
    // endpoint->sll_family = AF_PACKET;
    // endpoint->sll_protocol = htons(ETH_P_ARP);
    // endpoint->sll_hatype = htons(ARPHRD_ETHER);
    // endpoint->sll_pkttype = (PACKET_BROADCAST);
    // endpoint->sll_halen = 6; // MAC address len (host addr len)

    char* interface = "eno1"; // Ethernet Interface
    if((endpoint->sll_ifindex = if_nametoindex(interface)) == 0){
        fprintf(stderr, "Error converting interface name to index: %d\n", errno);
        perror("Errno");
        close(fd);
        return ERROR;
    }

    // inet_aton(ip, &endpoint->sll_addr); // Set destination ip
    
    // Enable broadcasting
    // int broadcastEnable=1;
    // int ret=setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

    // if(bind(fd, (struct sockaddr*)endpoint, sizeof(struct sockaddr_ll)) < 0){
    //     fprintf(stderr, "Error binding socket: %d\n", errno);
    //     perror("Errno");
    //     close(fd);
    //     return ERROR;
    // }

    return fd;
}

void udp_send_message(char* buff, size_t size, int udp_socket, struct sockaddr* endpoint){

    // Now simply send your message!
    ssize_t err = 0;
    if((err = sendto(udp_socket, buff, size, 0, endpoint, sizeof(struct sockaddr_ll))) < 0){
        fprintf(stderr, "Error - Unable to send RAW packet - Error Code: %zd\n", err);
        perror("Errno");
        return;
    }

    printf("Message Sent!\n");
}


void udp_cleanup(int udp_socket){
    // Cleaup
    close(udp_socket);
}