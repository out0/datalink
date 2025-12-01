#include "../include/tcplink.h"
#include <unistd.h>
#include <iostream>

#define SIZE 1024*1024

int main(int argc, char **argv)
{

    printf("Data Server\n");

    uint8_t *payload = new uint8_t[SIZE];
    for (int i = 0; i < SIZE; i++)
        payload[i] = i % 1024;

    TCPLink link(20000, 100);

    while (true)
    {
        if (!link.isReady())
        {
            printf("Waiting for the client to connect\n");
            while (!link.isReady())
                ;
            printf("client connected\n");
        }
        printf ("sending payload\n");
        link.write(payload, SIZE);
        
        //printf ("hit enter\n");
        //std::cin.get();
        sleep(0.01);
    }
}