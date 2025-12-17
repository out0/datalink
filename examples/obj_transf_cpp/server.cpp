#include "../../include/datalink.h"
#include <unistd.h>
#include <iostream>

class Example1 {
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

    printf("Data Server\n");
    auto payload_class = new Example1();
    payload_class->val1 = 123;
    payload_class->val2 = 'a';
    payload_class->val3 = 123.45;
    payload_class->val4 = 123.4567;
    payload_class->val5 = new char[5] { 'a', 'b', 'c', 'd', 'e' };
    payload_class->val7 = true;

    auto link = Datalink::TcpServer(20000, 100, 1);

    while (true)
    {
        if (!link->isReady())
        {
            printf("Waiting for the client to connect\n");
            while (!link->isReady()) {}
            printf("client connected\n");
        }
        printf ("sending payload\n");

        Serializer s(sizeof(Example1));
        s.write(payload_class, sizeof(Example1));
        s.write(payload_class->val5, 5 * sizeof(char));
        std::string str = "just a string";
        s.write(str);
        auto raw = s.get();

        link->writeWithAck(&raw[0], raw.size());
        sleep(0.01);
    }
}