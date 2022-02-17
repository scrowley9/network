#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h> // For print formatting
#include <arpa/inet.h>

#define ETH_P_ARP 0x0806 /* Address Resolution packet */
#define ARP_HTYPE_ETHER 1  /* Ethernet ARP type */
#define ARP_PTYPE_IPv4 0x0800 /* Internet Protocol packet */

#define MAC_ADDRESS_LEN 6 // MAC addresses are 6 bytes
#define IPv4_ADDRESS_LEN 4 // IPv4 addresses are 4 bytes

/**
 * NOTE: Make sure to ALIGN this memory!!!!
 * 
 */

/* Ethernet ARP packet from RFC 826 */
#pragma pack(1)
typedef struct {
   uint16_t htype;   /* Format of hardware address */
   uint16_t ptype;   /* Format of protocol address */
   uint8_t hlen;    /* Length of hardware address */
   uint8_t plen;    /* Length of protocol address */
   uint16_t op;    /* ARP opcode (command) */
   uint8_t sha[MAC_ADDRESS_LEN];  /* Sender hardware address */
   uint32_t spa;   /* Sender IP address */
   uint8_t tha[MAC_ADDRESS_LEN];  /* Target hardware address */
   uint32_t tpa;   /* Target IP address */
} arp_ether_ipv4;

/* Ethernet frame header */
#pragma pack(1)
typedef struct {
   uint8_t dest_addr[MAC_ADDRESS_LEN]; /* Destination hardware address */
   uint8_t src_addr[MAC_ADDRESS_LEN];  /* Source hardware address */
   uint16_t frame_type;   /* Ethernet frame type */
} ether_hdr;

uint8_t* create_arp_request(arp_ether_ipv4* arp);

arp_ether_ipv4* init_arp_struct(uint16_t htype, uint16_t ptype, uint8_t hlen, uint8_t plen, uint16_t op, uint8_t sha[MAC_ADDRESS_LEN], uint32_t spa, uint8_t tha[MAC_ADDRESS_LEN], uint32_t tpa);
uint8_t* convert_MAC_addr_to_bytes(char* mac_addr);
uint32_t convert_IP_addr_to_bytes(char* ip_addr);
char* uint32_to_ip_string(uint32_t ip_bytes);

void print_arp_packet(uint8_t* packet);
void print_arp_struct_bytes(arp_ether_ipv4* packet);
void print_arp_struct(arp_ether_ipv4* packet);
void packageARP(unsigned char *buffer, ether_hdr *frameHeader, arp_ether_ipv4 *arp_packet, size_t *bufferSize);
