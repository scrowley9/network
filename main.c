#include "client.h"
#include "arp.h"
#include "udp.h"

#include <unistd.h> // sleep

// Extra stuff I found online -- not my macros
# define NEWLINE			"\n"
# define ADD_NEWLINE(STRING)		STRING NEWLINE
# define ERROR_SOCKET_CREATION		ADD_NEWLINE("[-] ERROR: Socket creation failed")
# define ERROR_GET_MAC			ADD_NEWLINE("[-] ERROR: Could not get MAC address")
# define ERROR_PACKET_CREATION_ARP	ADD_NEWLINE("[-] ERROR: ARP packet creation failed")
# define ERROR_PACKET_CREATION_ETHER	ADD_NEWLINE("[-] ERROR: Ether frame creation failed")
# define ERROR_COULD_NOT_SEND		ADD_NEWLINE("[-] ERROR: Could not send")
# define ERROR_COULD_NOT_RECEIVE	ADD_NEWLINE("[-] ERROR: Could not receive")
# define ERROR_DISPLAY_USAGE		ADD_NEWLINE("[*] USAGE: %s source_ip target_ip interface")

# define PRINT_MAC_ADDRESS(X)   fprintf(stdout, "|MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n", \
					X[0],				\
					X[1],				\
					X[2],				\
					X[3],				\
					X[4],				\
					X[5]);

// uint8_t	*get_victim_response(const int sd, const char *victim_ip);

// Typical usage for 
void usage(const char prog_name[const])
{
  fprintf(stderr, ERROR_DISPLAY_USAGE, prog_name);
}

// char* google = "google.com";


char* unknown_mac = "ff:ff:ff:ff:ff:ff";
char* desination_ip = "192.168.1.1"; // Router IP

// char* host_mac = "f8:ff:c2:41:53:71";
char* host_mac = "d4:5d:64:27:52:81";
char* host_ip = "192.168.1.33"; // linux machine

int main(int argc, char const *argv[]){

    char *victim_ip, *spoofed_ip_source, *interface;
    uint8_t	*my_mac_address, *victim_mac_address;
    struct sockaddr_ll	device;
    int			sd;

    if (argc != 4){
        return (usage(argv[0]), EXIT_FAILURE);
    }

    /* Get command line args */
    spoofed_ip_source = (char*)argv[1]; victim_ip = (char*)argv[2]; interface = (char*)argv[3];

    // Create Socket
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
    uint8_t* msg = create_arp_request(arp); // Add etherenet header
    printf("HIT!2\n");

    print_arp_packet_bytes(msg);


    struct sockaddr_ll endpoint;
    int broadcast = create_broadcast_socket(desination_ip, &endpoint); // create udp socket and set endpoint ip
    udp_send_message(msg, sizeof(arp_ether_ipv4)+sizeof(ether_hdr), broadcast, (struct sockaddr*)&endpoint); // broadcast

    // get_victim_response(broadcast, desination_ip);

    close(broadcast);

    free(arp);
    free(host);
    free(dest);


    return 0;
}



/* NOT MY CODE -- found this online for recving ARP response */
// uint8_t	*get_victim_response(const int sd, const char *victim_ip)
// {
//   char				buffer[IP_MAXPACKET];
//   ether_hdr		*ethernet_packet;
//   arp_ether_ipv4			*arp_packet;
//   uint8_t			*victim_mac_address;
//   char				uint8_t_to_str[INET_ADDRSTRLEN] = {0};

//   if (!(victim_mac_address = malloc(sizeof(uint8_t) * 6)))
//     return (NULL);
//   fprintf(stdout, "[*] Listening for target response..\n");
//   while (1)
//     {
//       if (recvfrom(sd, buffer, IP_MAXPACKET, 0, NULL, NULL) <= 0)
// 	return (NULL);

//       ethernet_packet = (ether_hdr *)buffer;
//       if (ntohs(ethernet_packet->frame_type) != ETH_P_ARP)
// 	continue ;

//       arp_packet = (arp_ether_ipv4 *)(buffer + sizeof(ether_hdr));

//       if (ntohs(arp_packet->op) != ARPOP_REPLY
// 	  || (arp_packet->spa && !inet_ntop(AF_INET, &arp_packet->spa, uint8_t_to_str, INET_ADDRSTRLEN))
// 	  || strcmp(uint8_t_to_str, victim_ip))
// 	{
// 	  memset(uint8_t_to_str, 0, INET_ADDRSTRLEN);
// 	  continue ;
// 	}
//       fprintf(stdout, "[+] Got response from victim\n");
//       fprintf(stdout, "[*] Sender mac address: ");
//       PRINT_MAC_ADDRESS(arp_packet->sha);
//       fprintf(stdout, "[*] Sender ip address: ");
//     //   PRINT_IP_ADDRESS(&arp_packet->spa);
//       fprintf(stdout, "[*] Target mac address: ");
//       PRINT_MAC_ADDRESS(arp_packet->tha);
//       fprintf(stdout, "[*] Target ip address: ");
//     //   PRINT_IP_ADDRESS(&arp_packet->tpa);

//       memcpy(victim_mac_address, arp_packet->sha, 6 * sizeof(uint8_t));
//       fprintf(stdout, "[*] Victim's mac address: ");
//       PRINT_MAC_ADDRESS(victim_mac_address);
//       return (victim_mac_address);
//     }
// }
