#include "arp.h"

uint8_t* convert_MAC_addr_to_bytes(char* mac_addr);
uint32_t convert_IP_addr_to_bytes(char* ip_addr);
char* uint32_to_ip_string(uint32_t ip_bytes);