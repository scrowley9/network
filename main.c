#include "client.h"
#include "arp.h"

// char* google = "google.com";

arp_ether_ipv4* create_arp_struct(){
    
}

int main(int argc, char const *argv[])
{

    char* data = uint32_to_ip_string((uint32_t)0x1101a8c0);
    printf("%s\n", data);
    free(data);

    // char host[256];
    // printf("Type Hostname: ");
    // fscanf(stdin, "%s", host);

    // // Check host name, etc.

    // if(tcp_connection_loop(host) == ERROR){
    //     return ERROR;
    // }

    return 0;
}

