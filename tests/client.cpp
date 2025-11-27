#include "../include/tcplink.h"
#include <unistd.h>

#define SIZE 1024 * 1024

int main(int argc, char **argv)
{

    printf("Data client\n");

    TCPLink link("127.0.0.1", 20000, 100);

    int j = 9;
    while (j >= 0)
    {
        if (!link.isReady())
        {
            printf("Establishing a connection to the server...\n");
            while (!link.isReady())
                ;
            printf("connected\n");
        }
        
        if (link.hasData()) {
            auto data = link.readMessage();
            printf("received %ld bytes\n", data.size());
            //j--;
            // auto data = link.readRawMessage();
            // printf("received %ld bytes\n", data.size);
            // delete []data.data;
        }
        //sleep(0.1);
    }
}