#include "../../include/datalink.h"
#include "../../include/serializer.h"
#include <stdio.h>
#include <string>

class Example1
{
public:
    int val1;
    char val2;
    float val3;
    double val4;
    char *val5;
    bool val7;
};

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
            while (!link->isReady())
            {
            }
            printf("connected\n");
        }

        if (link->hasData())
        {
            auto [raw, timestamp] = link->readMessageWithAck();
            if (timestamp < 0 || raw.size() == 0)
            {
                printf("no object\n");
                continue;
            }
            Serializer s(raw);
            Example1 data;
            if (s.read_ptr(&data))
            {
                data.val5 = s.read_vector<char>();
                auto k = s.read_string();
                std::cout << "k: " << k << "\n";
                //data.val6 = k;
                printf("Received:\n\tval1=%d\n\tval2=%c\n\tval3=%f\n\tval4=%f\n\tval5=%s\n\tval6=%s\n\tval7=%d\n\ttimestamp=%f\n",
                       data.val1, data.val2, data.val3, data.val4, data.val5, k.c_str(), data.val7, timestamp);
                
            }
            // printf("Received:\n\tval1=%d\n\tval2=%c\n\tval3=%f\n\tval4=%f\n\tval5=%s\n\tval6=%s\n\tval7=%d\n\ttimestamp=%f\n",
            //     data.val1, data.val2, data.val3, data.val4, data.val5, data.val6.c_str(), data.val7, timestamp);
            // j--;
        }
    }
}