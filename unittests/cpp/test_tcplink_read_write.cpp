#include <gtest/gtest.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "../../include/tcplink.h"

extern void exec_start();
extern void exec_finished(const char *msg);


TEST(TCPLinkReadWrite, TestReadWriteNoTimeoutSuccess)
{
    return;
    TCPLink server(20000, -1);
    TCPLink client("127.0.0.1", 20000, -1);

    int max_loops = 10;
    while (max_loops >= 0)
    {
        if (server.isReady() && client.isReady())
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        max_loops--;
    }

    if (max_loops <= 0)
    {
        printf("Timout while trying to connect\n");
        FAIL();
    }

    int size = 1024 * 1024; // 1 MB
    char *payload = new char[size];
    for (int i = 0; i < size; i++)
    {
        payload[i] = i % 100;
    }

    printf("connected, sending data now\n");
    ASSERT_TRUE(client.write(payload, size));

    while (!server.hasData()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    printf("receiving the data (server)\n");
    auto data = server.readMessage();

    printf ("received %ld bytes\n", data.size());
    for (int i = 0; i < size; i++)
    {
        if (data[i] != payload[i])
            FAIL();
    }
}


TEST(TCPLinkReadWrite, TestReadWriteTimeoutSuccess)
{
    TCPLink server(20011, 50);
    TCPLink client("127.0.0.1", 20011, 50);

    int max_loops = 10;
    while (max_loops >= 0)
    {
        if (server.isReady() && client.isReady())
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        max_loops--;
    }

    if (max_loops <= 0)
    {
        printf("Timout while trying to connect\n");
        FAIL();
    }

    int size = 1024 * 1024; // 1 MB
    char *payload = new char[size];
    for (int i = 0; i < size; i++)
    {
        payload[i] = i % 100;
    }

    //printf("connected, sending data now\n");
    exec_start();
    ASSERT_TRUE(client.write(payload, size));
    exec_finished("1MB data send client->server");

    //printf("receiving the data (server)\n");
    while (!server.hasData()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    exec_start();
    auto data = server.readMessage();
    exec_finished("1MB data recv client->server");

    ASSERT_EQ(data.size(), size);
    for (int i = 0; i < size; i++)
    {
        if (data[i] != payload[i])
            FAIL();
    }
}
