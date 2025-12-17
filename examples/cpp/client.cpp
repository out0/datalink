#include "../../include/datalink.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    printf("Data client\n");
    auto link = Datalink::TcpClient("127.0.0.1", 20000, 100, 1);

    int j = 9;
    while (j >= 0)
    {
        if (!link->isReady())
        {
            printf("Establishing a connection to the server...\n");
            while (!link->isReady()) {}
            printf("connected\n");
        }
        
        if (link->hasData()) {
            auto [data, timestamp] = link->readMessage();
            printf("received %ld bytes [%f]\n", data.size(), timestamp);            
            link->writeKeepAlive();
        }
    }
}