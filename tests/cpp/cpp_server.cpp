#include "../../include/datalink.h"
#include <unistd.h>

#define SIZE 209710
#define PORT 21000
#define TIMEOUT 2

int main()
{

    auto link = Datalink(PORT, TIMEOUT);
    int data_count = 0;

    char *huge_data = new char[SIZE];
    for (int i = 0; i < SIZE; i++)
        huge_data[i] = i % 100;

    
    while (true)
    {
        if (!link.isReady()) {
            printf ("link not ready\n");
            while (!link.isReady()) {
                sleep(1);
                continue;
            }
        }
        printf("sending data %d\n", data_count);
        link.sendData(huge_data, SIZE);

        auto rcvd = link.receiveData(TIMEOUT);

        if (rcvd == nullptr) continue;
        printf("receiving data %ld\n", rcvd->size);

        data_count++;
        //sleep(1);
    }

    delete []huge_data;
}