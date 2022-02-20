#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h> // For print formatting
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <errno.h>

#define ETH_P_ARP 0x0806 /* Address Resolution packet */
#define ARP_HTYPE_ETHER 1  /* Ethernet ARP type */
#define ARP_PTYPE_IPv4 0x0800 /* Internet Protocol packet */

#define MAC_ADDRESS_LEN 6 // MAC addresses are 6 bytes
#define IPv4_ADDRESS_LEN 4 // IPv4 addresses are 4 bytes

#define SUCCESS 0
#define ERROR -1

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

int send_request(char* src_mac, char* dst_mac, char* src_ip, char* dst_ip, char* interface);
uint8_t	*recv_reply(const int sd, const char *victim_ip);
