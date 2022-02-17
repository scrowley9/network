#include "arp.h"

uint8_t* create_arp_request(arp_ether_ipv4* arp){

    ether_hdr eth0;
    uint8_t broad[6] = {-1,-1,-1,-1,-1,-1};
    memcpy(eth0.dest_addr, broad, 6); // destination broadcast
    memcpy(eth0.src_addr, arp->sha, 6); // src
    eth0.frame_type = htons(0x0806); // Specifies ARP message

    // Create Ethernet Buffer
    uint8_t* buff = calloc(1, sizeof(ether_hdr)+sizeof(arp_ether_ipv4));
    if(!buff){
        fprintf(stderr, "Error - Failed to allocate for arp request buffer\n");
    }

    memcpy(buff, &eth0, sizeof(ether_hdr)); // Copy ethernet header into buffer

    // change to network byte order
    arp->htype = ntohs(arp->htype);
    arp->ptype = ntohs(arp->ptype);
    arp->op = ntohs(1);
    
    memcpy(buff+sizeof(ether_hdr), arp, sizeof(arp_ether_ipv4)); // Copy arp header into buffer

    return (uint8_t*)buff;
}

arp_ether_ipv4* init_arp_struct(uint16_t htype, uint16_t ptype, uint8_t hlen, uint8_t plen, uint16_t op, uint8_t sha[MAC_ADDRESS_LEN], uint32_t spa, uint8_t tha[MAC_ADDRESS_LEN], uint32_t tpa){
    
    arp_ether_ipv4* msg = calloc(1, sizeof(arp_ether_ipv4));
    if(!msg){
        return NULL;
    }

    msg->htype = htype; // ntohs
    msg->ptype = ptype; // ntohs
    msg->hlen = hlen;
    msg->plen = plen;
    msg->op = op; // ntohs
    memcpy(msg->sha, sha, MAC_ADDRESS_LEN);
    msg->spa = spa;
    memcpy(msg->tha, tha, MAC_ADDRESS_LEN);
    msg->tpa = tpa;

    return msg;
}

uint8_t* convert_MAC_addr_to_bytes(char* mac_addr){
    
    uint8_t* mac_byte_addr = calloc(6, sizeof(uint8_t));
    if(!mac_byte_addr){
        fprintf(stderr, "Error - allocating for MAC address\n");
        return NULL;
    }

    // MAC String to Bytes
    int num_bytes = 6;
    for(int i = 0; i < num_bytes; i++){
        uint8_t byte = 0;
        sscanf(mac_addr, "%x", &mac_byte_addr[i]); // read two chars and save as byte

        // move temp ptr to next byte
        if((mac_addr = strchr(mac_addr, (int)':')) == NULL) break; // just in case
        mac_addr+=1;
    }

    return mac_byte_addr;
}

uint32_t convert_IP_addr_to_bytes(char* ip_addr){
    uint32_t ip = 0;
    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET; // unnecessary
    my_addr.sin_port = htons(80); // unnecessary

    inet_aton(ip_addr, &(my_addr.sin_addr)); // inet_aton > inet_addr("ip")

    return (uint32_t)my_addr.sin_addr.s_addr; // return uint32_t

    // uint32_t ip_bytes = 0;
    // int num_bytes = 4;
    // for(int i = 0; i < num_bytes; i++){
    //     uint8_t byte = 0;
    //     sscanf(ip_addr, "%d", &byte); // read two chars and save as byte
    //     ip_bytes = (ip_bytes << 8) | byte;

    //     // move temp ptr to next byte
    //     if((ip_addr = strchr(ip_addr, (int)'.')) == NULL) break; // just in case
    //     ip_addr+=1;
    // }
    // return ip_bytes;
}

/* Remember to free returned data */
char* uint32_to_ip_string(uint32_t ip_bytes){

    struct in_addr ip;
    ip.s_addr = ip_bytes;
    char* data = inet_ntoa(ip); // returns a pointer to a static buffer that is overwritten each time its called
    size_t len = strlen(data);
    char* ret = calloc(len+1, sizeof(char));
    if(!ret){
        fprintf(stderr, "Failed to allocate IP address buffer\n");
        return NULL;
    }

    // Save IP Address Buffer
    memcpy(ret, data, len);
    
    return ret;
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

void packageARP(unsigned char *buffer, ether_hdr *frameHeader, arp_ether_ipv4 *arp_packet, size_t *bufferSize) {
  unsigned char *cp;
  size_t packet_size = sizeof(frameHeader) + sizeof(arp_ether_ipv4);

  cp = buffer;
  
  /*
   *  Copy the Ethernet frame header to the buffer.
   */
  memcpy(cp, &(frameHeader->dest_addr), sizeof(frameHeader->dest_addr));
  cp += sizeof(frameHeader->dest_addr);

  memcpy(cp, &(frameHeader->src_addr), sizeof(frameHeader->src_addr));
  cp += sizeof(frameHeader->src_addr);

  /* Normal Ethernet-II framing */
  memcpy(cp, &(frameHeader->frame_type), sizeof(frameHeader->frame_type));
  cp += sizeof(frameHeader->frame_type);


  /*
   *  Add the ARP data.
   */
  memcpy(cp, &(arp_packet->htype), sizeof(arp_packet->htype));
  cp += sizeof(arp_packet->htype);

  memcpy(cp, &(arp_packet->ptype), sizeof(arp_packet->ptype));
  cp += sizeof(arp_packet->ptype);

  memcpy(cp, &(arp_packet->hlen), sizeof(arp_packet->hlen));
  cp += sizeof(arp_packet->hlen);

  memcpy(cp, &(arp_packet->plen), sizeof(arp_packet->plen));
  cp += sizeof(arp_packet->plen);

  memcpy(cp, &(arp_packet->op), sizeof(arp_packet->op));
  cp += sizeof(arp_packet->op);

  memcpy(cp, &(arp_packet->sha), sizeof(arp_packet->sha));
  cp += sizeof(arp_packet->sha);

  memcpy(cp, &(arp_packet->spa), sizeof(arp_packet->spa));
  cp += sizeof(arp_packet->spa);

  memcpy(cp, &(arp_packet->tha), sizeof(arp_packet->tha));
  cp += sizeof(arp_packet->tha);

  memcpy(cp, &(arp_packet->tpa), sizeof(arp_packet->tpa));
  cp += sizeof(arp_packet->tpa);

  *bufferSize = packet_size;
}
