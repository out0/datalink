#include "../include/databridge.h"
#include <unistd.h>
#include <iostream>

int main(int argc, char **argv)
{
    printf("Data Bridge\n");

    // serves any client trying to reach port 20000
    auto left = Datalink::TcpServer(20000, 100);
    // connects to the client on port 21000
    auto right = Datalink::TcpClient("127.0.0.1", 21000, 100);
   
    auto bridge = DataBrigde(left.get(), right.get());

    printf ("hit enter to kill the bridge\n");
    std::cin.get();
}