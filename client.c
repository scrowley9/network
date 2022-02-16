#include "client.h"

/**
 * Pulled from:
 *  URL: https://pubs.opengroup.org/onlinepubs/7908799/xns/syssocket.h.html
 *  URL: https://www.geeksforgeeks.org/socket-programming-cc/
 */
static char* get_ip_from_host(char* host);

char buff[1024];

/**
 * @brief return fd of connected client socket
 */
int tcp_connection_loop(char* host){

    char* ip = get_ip_from_host(host);
    if(!ip){
        return ERROR;
    }
    printf("Hostname: %s\t-->\tIP Address: %s\n", host, ip);


    // Create Socket
    // int domain = AF_UNIX; // connect to local host (pipes)
    // int data_type = SOCK_DGRAM; // UDP - datagram

    memset(buff, 0, 1024);
    int domain = AF_INET; // connect to internet
    int data_type = SOCK_STREAM; // TCP - stream
    int protocol = 0; // Use IP - This is the same number which appears on protocol field in the IP header of a packet.
    int fd = -1;

    if((fd = socket(domain, data_type, protocol)) < 0){
        // Failed!
        fprintf(stderr, "Error - Unable to create socket!\n");
        return ERROR;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET; // connect to internet
    serv_addr.sin_port = htons(HTTP_PORT); // htons - converts values between host and network byte order
    
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0){
        fprintf(stderr, "Error - Address not supported\n");
        close(fd);
        return -1;
    }

    if (connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        fprintf(stderr, "Error - Unable to connect\n");
        close(fd);
        return -1;
    }

    int alive = 1;
    char str[1024];

    char* message = "GET / HTTP/1.1\r\n\r\n";

    do{
        // printf("Type a message: ");
        // fscanf(stdin, "%s", str);
        // if(str[0] == '0')
        //     break;
        send(fd, message, strlen(message), 0);
        printf("Message Sent\n");
        read(fd, buff, sizeof(buff));
        printf("Message Received\n\n%s\n", buff);
        printf("\n\nKeep Alive? (1 - yes, 0 - no): ");
        fscanf(stdin, "%d", &alive);
        printf("\n");
    }while(alive);

    // Cleaup
    close(fd);
    free(ip);
    return 0;
}


/**
 * @brief Translate host name to IP and return the first ip
 * 
 * @param host 
 * @return char* 
 */
static char* get_ip_from_host(char* host){
    // Translate hostname to IP
    struct addrinfo hints;
    struct addrinfo *res, *tmp; // Dynamically allocated in getaddrinfo
    char ip_address[20];

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    int ret = 0;

    if((ret = getaddrinfo(host, NULL, &hints, &res))){
        fprintf(stderr, "Error - getaddrinfo: %s\n", gai_strerror(ret));
        return NULL;
    }
    
    // Get First IP address
    tmp = res;
    getnameinfo(tmp->ai_addr, tmp->ai_addrlen, ip_address, sizeof(ip_address), NULL, 0, NI_NUMERICHOST);

    // Copy IP out of function
    char* out_buff = calloc(20, sizeof(char));
    if(!out_buff){
        fprintf(stderr, "Error - allocating for IP address\n");
        freeaddrinfo(res);
        return NULL;
    }
    memcpy(out_buff, ip_address, sizeof(ip_address));

    // Cleanup
    freeaddrinfo(res);

    return out_buff;
}

