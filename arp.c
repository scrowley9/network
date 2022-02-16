#include "arp.h"
#include <inttypes.h> // For print formatting
#include <arpa/inet.h>

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

void print_arp_packet_bytes(arp_ether_ipv4* packet){
    uint8_t* p = (uint8_t*)packet;
    for(int i = 0; sizeof(arp_ether_ipv4); i++){
        printf("%"PRIu8"", p[i]);
    }
    printf("\n");
}

void print_arp_packet(arp_ether_ipv4* packet){
    printf("Hardware Type: %"PRIu16"\n", packet->htype);
    printf("Protocol Type: %"PRIu16"\n", packet->ptype);
    printf("Hardware Size: %"PRIu8"\n", packet->hlen);
    printf("Protocol Size: %"PRIu8"\n", packet->plen);

    printf("Opcode: %"PRIu16"\n", packet->op);

    uint8_t a, b, c, d, e, f;
    a = packet->sha[0];
    b = packet->sha[1];
    c = packet->sha[2];
    d = packet->sha[3];
    e = packet->sha[4];
    f = packet->sha[5];
    printf("Sender MAC: %"PRIu8"%"PRIu8"%"PRIu8"%"PRIu8"%"PRIu8"%"PRIu8"\n", a, b, c, d, e, f);
    
    char* data = uint32_to_ip_string(packet->spa);
    printf("Sender IP: %s\n", data);
    free(data);

    a = packet->tha[0];
    b = packet->tha[1];
    c = packet->tha[2];
    d = packet->tha[3];
    e = packet->tha[4];
    f = packet->tha[5];
    printf("Receiver MAC: %"PRIu8"%"PRIu8"%"PRIu8"%"PRIu8"%"PRIu8"%"PRIu8"", a, b, c, d, e, f);
    data = uint32_to_ip_string(packet->tpa);
    printf("Receiver IP: %s", data);
    free(data);
}

void packageARP(unsigned char *buffer, ether_hdr *frameHeader, arp_ether_ipv4 *arp_packet, size_t *bufferSize) {
  unsigned char *cp;
  size_t packet_size = sizeof(arp_ether_ipv4);

  cp = buffer;

//   packet_size = sizeof(frameHeader->dest_addr) 
//                 + sizeof(frameHeader->src_addr)  
//                 + sizeof(frameHeader->frame_type)
//                 + sizeof(arp_packet->htype)       
//                 + sizeof(arp_packet->ptype)    
//                 + sizeof(arp_packet->hlen)        
//                 + sizeof(arp_packet->plen)       
//                 + sizeof(arp_packet->op)        
//                 + sizeof(arp_packet->sha)        
//                 + sizeof(arp_packet->spa)         
//                 + sizeof(arp_packet->tha)        
//                 + sizeof(arp_packet->tpa);
  
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
