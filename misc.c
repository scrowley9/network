#include "misc.h"

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
        sscanf(mac_addr, "%hhx", &mac_byte_addr[i]); // read two chars and save as byte

        // move temp ptr to next byte
        if((mac_addr = strchr(mac_addr, (int)':')) == NULL) break; // just in case
        mac_addr+=1;
    }

    return mac_byte_addr;
}

uint32_t convert_IP_addr_to_bytes(char* ip_addr){
    uint32_t ip = 0;
    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_PACKET; // unnecessary
    my_addr.sin_port = htons(80); // unnecessary

    inet_aton(ip_addr, &(my_addr.sin_addr)); // inet_aton > inet_addr("ip")

    return (uint32_t)my_addr.sin_addr.s_addr; // return uint32_t

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
