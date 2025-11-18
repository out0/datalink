#include "../../include/datalink.h"
#include <cstring>
#include <unistd.h>

#define SIZE 209710000
#define TIMEOUT 2
int main()
{
    auto link = new Datalink("127.0.0.1", 21000, TIMEOUT);
    int data_count = 0;

    char *huge_data = new char[SIZE];
    for (int i = 0; i < SIZE; i++)
        huge_data[i] = i % 100;

    while (true)
    {
        auto res = link->receiveData(TIMEOUT);
        if (res == nullptr)
            continue;

        printf("received data %ld data_count: %d\n", res->size, data_count);

        printf("sending data...\n");
        link->sendData(huge_data, SIZE);

        // if (res->size != 2097100)
        // {
        //     printf("size mismatch\n");
        //     exit(1);
        // }

        // for (int i = 0; i < SIZE; i++)
        // {
        //     if (res->data[i] != i % 100)
        //     {
        //         printf("data is corrupt %d != %d\n", i, res->data[i]);
        //         exit(1);
        //     }
        // }
        data_count++;
    }

    delete link;
}