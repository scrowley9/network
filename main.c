#include "client.h"
#include "arp.h"

// char* google = "google.com";
char* unknown_mac = "00:00:00:00:00:00";
char* known_ip = "192.168.1.1";

char* host_mac = "f8:ff:c2:41:53:71";
char* host_ip = "192.168.1.17";

int main(int argc, char const *argv[]){

    uint8_t* host = convert_MAC_addr_to_bytes(host_mac);
    uint8_t* dest = convert_MAC_addr_to_bytes(unknown_mac);

    arp_ether_ipv4* arp = create_arp_message(
                                            ARP_HTYPE_ETHER,
                                            ARP_PTYPE_IPv4,
                                            MAC_ADDRESS_LEN,
                                            IPv4_ADDRESS_LEN,
                                            1,
                                            host,
                                            convert_IP_addr_to_bytes(host_ip),
                                            dest,
                                            convert_IP_addr_to_bytes(known_ip));

    print_arp_packet(arp);
    printf("\n%lu\n\n", sizeof(arp_ether_ipv4));
    print_arp_packet_bytes(arp);

    free(arp);
    free(host);
    free(dest);

    // char* data = uint32_to_ip_string((uint32_t)0x1101a8c0);
    // printf("%s\n", data);
    // free(data);

    // char host[256];
    // printf("Type Hostname: ");
    // fscanf(stdin, "%s", host);

    // // Check host name, etc.

    // if(tcp_connection_loop(host) == ERROR){
    //     return ERROR;
    // }

    return 0;
}

