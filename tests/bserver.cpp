#include "../include/datalink.h"
#include <unistd.h>
#include <iostream>

#define SIZE 1024*1024

int main(int argc, char **argv)
{

    printf("Data Server in port 21000 (unreachable from port 20000)\n");

    uint8_t *payload = new uint8_t[SIZE];
    for (int i = 0; i < SIZE; i++)
        payload[i] = i % 1024;

    auto link = Datalink::TcpServer(21000, 100);

    while (true)
    {
        if (!link->isReady())
        {
            printf("Waiting for the client to connect\n");
            while (!link->isReady()) {}
            printf("client connected\n");
        }
        printf ("sending payload\n");
        link->write(payload, SIZE, 123.45);

        auto [k, t] = link->readMessage();        
        //printf ("hit enter\n");
        //std::cin.get();
        sleep(0.01);
    }
}