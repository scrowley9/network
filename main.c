#include "client.h"

// char* google = "google.com";

int main(int argc, char const *argv[])
{
    char host[256];
    printf("Type Hostname: ");
    fscanf(stdin, "%s", host);

    // Check host name, etc.

    if(tcp_connection_loop(host) == ERROR){
        return ERROR;
    }
    
    return 0;
}

