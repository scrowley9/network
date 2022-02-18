#include "client.h"
#include "arp.h"
#include "udp.h"

#include <unistd.h> // sleep

// char* google = "google.com";

char* unknown_mac = "00:00:00:00:00:00";
char* desination_ip = "192.168.1.255";

char* host_mac = "f8:ff:c2:41:53:71";
char* host_ip = "192.168.1.17";

int main(int argc, char const *argv[]){

    uint8_t* host = convert_MAC_addr_to_bytes(host_mac);
    uint8_t* dest = convert_MAC_addr_to_bytes(unknown_mac);

    arp_ether_ipv4* arp = init_arp_struct(
                                            ARP_HTYPE_ETHER,
                                            ARP_PTYPE_IPv4,
                                            MAC_ADDRESS_LEN,
                                            IPv4_ADDRESS_LEN,
                                            1,
                                            host,
                                            convert_IP_addr_to_bytes(host_ip),
                                            dest,
                                            convert_IP_addr_to_bytes(desination_ip));

    printf("HIT!1\n");
    uint8_t* msg = create_arp_request(arp);
    printf("HIT!2\n");

    print_arp_packet_bytes(msg);

    // for(int i = 0; i < 800; i++){
        // sleep(1);
        struct sockaddr_in endpoint;
        int broadcast = create_broadcast_socket(17, desination_ip, &endpoint); // create udp socket and set endpoint ip
        udp_send_message(msg, sizeof(arp_ether_ipv4)+sizeof(ether_hdr), broadcast, &endpoint); // broadcast
        close(broadcast);

        broadcast = create_broadcast_socket(27, desination_ip, &endpoint); // create udp socket and set endpoint ip
        udp_send_message(msg, sizeof(arp_ether_ipv4)+sizeof(ether_hdr), broadcast, &endpoint); // broadcast
        close(broadcast);

        broadcast = create_broadcast_socket(30, desination_ip, &endpoint); // create udp socket and set endpoint ip
        udp_send_message(msg, sizeof(arp_ether_ipv4)+sizeof(ether_hdr), broadcast, &endpoint); // broadcast
        close(broadcast);
        
        // 17, 27, 30
        // AF_ROUTE - 17 ()
        // AF_NDRV - 27 "raw" access to network driver
        // AF_INET6 - 30 (IPv6)
    // }
    // for(int i = 0; i < 10; i++){
    //     sleep(1);
    //     printf("Retransmitting\n");
    //     udp_send_message(msg, sizeof(arp_ether_ipv4)+sizeof(ether_hdr), broadcast, &endpoint); // broadcast
    // }

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

