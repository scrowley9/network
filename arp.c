#include "arp.h"
#include "misc.h"

uint8_t* create_arp_request(char* src_mac, char* dst_mac, char* src_ip, char* dst_ip);
arp_ether_ipv4* init_arp_struct(uint16_t htype, uint16_t ptype, uint8_t hlen, uint8_t plen, uint16_t op, uint8_t sha[MAC_ADDRESS_LEN], uint32_t spa, uint8_t tha[MAC_ADDRESS_LEN], uint32_t tpa);
uint8_t* convert_MAC_addr_to_bytes(char* mac_addr);
uint32_t convert_IP_addr_to_bytes(char* ip_addr);
char* uint32_to_ip_string(uint32_t ip_bytes);

void print_arp_packet_bytes(uint8_t* packet);
void print_arp_struct_bytes(arp_ether_ipv4* packet);
void print_arp_struct(arp_ether_ipv4* packet);

/**
 * @brief Create a broadcast socket fd
 * REQUIRES: user to be root
 * 
 * @param ip 
 * @param endpoint 
 * @return int 
 */
int send_request(char* src_mac, char* dst_mac, char* src_ip, char* dst_ip, char* interface){

    // Create ARP Request Packet
    uint8_t* msg = create_arp_request(src_mac, dst_mac, src_ip, dst_ip);

    // Create Socket
    int domain = AF_PACKET; // AF_PACKET replacement (Linux only) -- send packet as is without transport protocol (i.e. no TCP header)
    int data_type = SOCK_RAW; // UDP socket -- don't use DGRAM because we want to fill in the ethernet header
    int protocol = htons(ETH_P_ARP); // ARP protocol
    int fd = -2;

    if((fd = socket(domain, data_type, protocol)) < 0){
        // MUST BE ROOT to create raw
        perror("Unable to create RAW socket (make sure to execute as admin)");
        free(msg);
        return ERROR;
    }

    // Find & Select Network Interface
    struct sockaddr_ll endpoint;
    memset(&endpoint, 0, sizeof(struct sockaddr_ll)); // clear out endpoint address
    
    if((endpoint.sll_ifindex = if_nametoindex(interface)) == 0){
        perror("Error converting interface name to index");
        close(fd);
        free(msg);
        return ERROR;
    }

    // Send ARP request
    ssize_t err = 0;
    if((err = sendto(fd, msg, sizeof(arp_ether_ipv4)+sizeof(ether_hdr), 0, (struct sockaddr*)&endpoint, sizeof(struct sockaddr_ll))) < 0){
        perror("Unable to send RAW packet");
        free(msg);
        close(fd);
        return ERROR;
    }

    printf("ARP Request Sent\n");
    free(msg);

    return fd;
}

/**
 * @brief Create buffer with Ethernet and ARP header
 * 
 * @param src_mac 
 * @return uint8_t* 
 */
uint8_t* create_arp_request(char* src_mac, char* dst_mac, char* src_ip, char* dst_ip){

    uint8_t* host = convert_MAC_addr_to_bytes(src_mac); // Get host mac address differently
    uint8_t* dest = convert_MAC_addr_to_bytes(dst_mac);

    // Create ARP header
    arp_ether_ipv4* arp = init_arp_struct(
                                        ARP_HTYPE_ETHER,
                                        ARP_PTYPE_IPv4,
                                        MAC_ADDRESS_LEN,
                                        IPv4_ADDRESS_LEN,
                                        1,
                                        host,
                                        convert_IP_addr_to_bytes(src_ip),
                                        dest,
                                        convert_IP_addr_to_bytes(dst_ip));

    // Create Ethernet Header
    ether_hdr eth0;
    uint8_t broad[6] = {-1,-1,-1,-1,-1,-1};
    memcpy(eth0.dest_addr, broad, 6); // destination broadcast
    memcpy(eth0.src_addr, arp->sha, 6); // src
    eth0.frame_type = htons(ETH_P_ARP); // Specifies ARP message

    // Create Packet Buffer
    uint8_t* buff = calloc(1, sizeof(ether_hdr)+sizeof(arp_ether_ipv4));
    if(!buff){
        fprintf(stderr, "Error - Failed to allocate for arp request buffer\n");
    }

    // Create Packet
    memcpy(buff, &eth0, sizeof(ether_hdr)); // Copy ethernet header into buffer
    memcpy(buff+sizeof(ether_hdr), arp, sizeof(arp_ether_ipv4)); // Copy arp header into buffer

    // Cleanups
    free(arp);
    free(host);
    free(dest);

    // print_arp_packet_bytes(buff);

    return (uint8_t*)buff;
}

arp_ether_ipv4* init_arp_struct(uint16_t htype, uint16_t ptype, uint8_t hlen, uint8_t plen, uint16_t op, uint8_t sha[MAC_ADDRESS_LEN], uint32_t spa, uint8_t tha[MAC_ADDRESS_LEN], uint32_t tpa){
    
    arp_ether_ipv4* msg = calloc(1, sizeof(arp_ether_ipv4));
    if(!msg){
        return NULL;
    }

    msg->htype = htons(htype); // htons
    msg->ptype = htons(ptype); // htons
    msg->hlen = hlen;
    msg->plen = plen;
    msg->op = htons(op); // htons
    memcpy(msg->sha, sha, MAC_ADDRESS_LEN);
    memcpy(msg->tha, tha, MAC_ADDRESS_LEN);
    msg->spa = spa;
    msg->tpa = tpa;

    return msg;
}

// Once ethernet
void print_arp_packet_bytes(uint8_t* packet){

    for(int i = 0; i < sizeof(ether_hdr)+sizeof(arp_ether_ipv4); i+=1){
        printf("%02x", packet[i]);
        if(i%4==0){
            printf("\n");
        }
    }
    printf("\n");
}

void print_arp_struct_bytes(arp_ether_ipv4* packet){
    uint8_t* p = (uint8_t*)packet;
    for(int i = 0; i < sizeof(arp_ether_ipv4); i+=2){
        printf("%02x%02x\n", p[i], p[i+1]);
    }
    printf("\n");
}

void print_arp_struct(arp_ether_ipv4* packet){
    printf("Hardware Type: %04hx\n", packet->htype);
    printf("Protocol Type: %04hx\n", packet->ptype);
    printf("Hardware Size: %02x\n", packet->hlen);
    printf("Protocol Size: %02x\n", packet->plen);

    printf("Opcode: %04hx\n", packet->op);

    uint8_t a, b, c, d, e, f;
    a = packet->sha[0];
    b = packet->sha[1];
    c = packet->sha[2];
    d = packet->sha[3];
    e = packet->sha[4];
    f = packet->sha[5];
    printf("Sender MAC: %"PRIx8":%"PRIx8":%"PRIx8":%"PRIx8":%"PRIx8":%"PRIx8"\n", a, b, c, d, e, f);
    
    char* data = uint32_to_ip_string(packet->spa);
    printf("Sender IP: %s\n", data);
    free(data);

    a = packet->tha[0];
    b = packet->tha[1];
    c = packet->tha[2];
    d = packet->tha[3];
    e = packet->tha[4];
    f = packet->tha[5];
    printf("Receiver MAC: %"PRIx8":%"PRIx8":%"PRIx8":%"PRIx8":%"PRIx8":%"PRIx8"\n", a, b, c, d, e, f);
    data = uint32_to_ip_string(packet->tpa);
    printf("Receiver IP: %s\n", data);
    free(data);
}


// Extra stuff I found online -- not my macros
# define NEWLINE			"\n"
# define ADD_NEWLINE(STRING)		STRING NEWLINE
# define ERROR_SOCKET_CREATION		ADD_NEWLINE("[-] ERROR: Socket creation failed")
# define ERROR_GET_MAC			ADD_NEWLINE("[-] ERROR: Could not get MAC address")
# define ERROR_PACKET_CREATION_ARP	ADD_NEWLINE("[-] ERROR: ARP packet creation failed")
# define ERROR_PACKET_CREATION_ETHER	ADD_NEWLINE("[-] ERROR: Ether frame creation failed")
# define ERROR_COULD_NOT_SEND		ADD_NEWLINE("[-] ERROR: Could not send")
# define ERROR_COULD_NOT_RECEIVE	ADD_NEWLINE("[-] ERROR: Could not receive")

# define PRINT_MAC_ADDRESS(X)   fprintf(stdout, "%02X:%02X:%02X:%02X:%02X:%02X\n", \
					X[0],				\
					X[1],				\
					X[2],				\
					X[3],				\
					X[4],				\
					X[5]);

uint8_t	*recv_reply(const int sd, const char *victim_ip)
{
    char				buffer[IP_MAXPACKET];
    ether_hdr		*ethernet_packet;
    arp_ether_ipv4			*arp_packet;
    uint8_t			*victim_mac_address;
    char				uint8_t_to_str[INET_ADDRSTRLEN] = {0};

    if (!(victim_mac_address = malloc(sizeof(uint8_t) * 6)))
        return (NULL);
    fprintf(stdout, "[*] Listening for target response..\n");

    while (1)
    {
        if (recvfrom(sd, buffer, IP_MAXPACKET, 0, NULL, NULL) < 0){
            perror("Unable to receive a packet");
            return (NULL);
        }
        ethernet_packet = (ether_hdr *)buffer;
        if (ntohs(ethernet_packet->frame_type) != ETH_P_ARP)
            continue ;

        arp_packet = (arp_ether_ipv4 *)(buffer + sizeof(ether_hdr));

        if (ntohs(arp_packet->op) != ARPOP_REPLY
            || (arp_packet->spa && !inet_ntop(AF_INET, &arp_packet->spa, uint8_t_to_str, INET_ADDRSTRLEN))
            || strcmp(uint8_t_to_str, victim_ip)){
            memset(uint8_t_to_str, 0, INET_ADDRSTRLEN);
            continue ;
        }
        fprintf(stdout, "[+] Got response from victim\n");
        fprintf(stdout, "[*] Sender mac address: ");
        PRINT_MAC_ADDRESS(arp_packet->sha);

        char* data = uint32_to_ip_string(arp_packet->spa);
        printf("[*] Sender IP address: %s\n", data);
        free(data);

        fprintf(stdout, "[*] Target mac address: ");
        PRINT_MAC_ADDRESS(arp_packet->tha);

        data = uint32_to_ip_string(arp_packet->tpa);
        printf("[*] Receiver IP address: %s\n\n", data);
        free(data);

        memcpy(victim_mac_address, arp_packet->sha, 6 * sizeof(uint8_t));
        fprintf(stdout, "[*] Victim's mac address: ");
        PRINT_MAC_ADDRESS(victim_mac_address);
        return (victim_mac_address);
    }
}
