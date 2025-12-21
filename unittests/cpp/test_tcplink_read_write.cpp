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

    int max_loops = 100;
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
    uint8_t *payload = new uint8_t[size];
    for (int i = 0; i < size; i++)
    {
        payload[i] = i % 100;
    }

    printf("connected, sending data now\n");
    ASSERT_TRUE(client.write(payload, size, 123.45));

    while (!server.hasData())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    printf("receiving the data (server)\n");
    auto [data, timestamp] = server.readMessage();

    printf("received %ld bytes\n", data.size());
    for (int i = 0; i < size; i++)
    {
        if (data[i] != payload[i])
            FAIL();
    }
    ASSERT_FLOAT_EQ(timestamp, 123.45);
}

TEST(TCPLinkReadWrite, TestReadWriteTimeoutSuccess)
{
    
    TCPLink server(20011, 50);
    TCPLink client("127.0.0.1", 20011, 50);

    int max_loops = 1000;
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
    uint8_t *payload = new uint8_t[size];
    for (int i = 0; i < size; i++)
    {
        payload[i] = i % 100;
    }

    // printf("connected, sending data now\n");
    exec_start();
    ASSERT_TRUE(client.write(payload, size, 123.45));
    exec_finished("1MB data send client->server");

    // printf("receiving the data (server)\n");
    while (!server.hasData())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    exec_start();
    auto [data, timestamp] = server.readMessage();
    exec_finished("1MB data recv client->server");

    ASSERT_EQ(data.size(), size);
    ASSERT_FLOAT_EQ(timestamp, 123.45);
    for (int i = 0; i < size; i++)
    {
        if (data[i] != payload[i])
            FAIL();
    }
}

TEST(TCPLinkReadWrite, TestReadWriteSequence)
{
    
    TCPLink server(20011, 200);
    TCPLink client("127.0.0.1", 20011, 200);

    int max_loops = 1000;
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

    int size = 1024; // 1 MB
    uint8_t *payload = new uint8_t[size];
    for (int i = 0; i < size; i++)
    {
        payload[i] = 0;
    }

    // printf("connected, sending data now\n");
    exec_start();
    for (int i = 0; i < 100; i++)
    {
        payload[0] = i;
        ASSERT_TRUE(client.write(payload, size, 123.45));
    }
    exec_finished("write sequence sent client->server");

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    // printf("receiving the data (server)\n");
    if (!server.hasData())
    {
        FAIL();
    }
    exec_start();
    int pos = 0;
    while (server.hasData())
    {
        auto [data, timestamp] = server.readMessage();
        // exec_finished("read data");

        ASSERT_EQ(data.size(), size);
        ASSERT_FLOAT_EQ(timestamp, 123.45);
        for (int i = 1; i < size; i++)
        {
            if (data[i] != 0)
            {
                printf("data #%d failed: data[%d] (%d) != 0\n", pos, i, data[i]);
                FAIL();
            }
        }
        if (data[0] != pos)
        {
            printf("data #%d failed: data[0] (%d) != %d\n", pos, data[0], pos);
        }
        // printf ("data #%d ok\n", pos);

        pos++;
    }
}

TEST(TCPLinkReadWrite, TestReadWriteOnceReadSequence)
{
    TCPLink server(20011, 200);
    TCPLink client("127.0.0.1", 20011, 200);

    int max_loops = 1000;
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

    int size = 1024; // 1 MB
    uint8_t *payload = new uint8_t[size];
    for (int i = 0; i < size; i++)
    {
        payload[i] = 0;
    }

    // printf("connected, sending data now\n");
    exec_start();
    payload[0] = 14;
    ASSERT_TRUE(client.write(payload, size, 123.45));
    exec_finished("write sequence sent client->server");

    // printf("receiving the data (server)\n");
    while (!server.hasData())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    exec_start();
    int pos = 0;
    while (server.hasData())
    {
        auto [data, timestamp] = server.readMessage();
        // exec_finished("read data");

        ASSERT_EQ(data.size(), size);
        ASSERT_FLOAT_EQ(timestamp, 123.45);
        for (int i = 1; i < size; i++)
        {
            if (data[i] != 0)
            {
                printf("data #%d failed: data[%d] (%d) != 0\n", pos, i, data[i]);
                FAIL();
            }
        }
        if (data[0] != 14)
        {
            printf("data #%d failed: data[0] (%d) != %d\n", pos, data[0], pos);
        }
        // printf ("data #%d ok\n", pos);
        pos++;
    }

    ASSERT_EQ(pos, 1);
}

TEST(TCPLinkReadWrite, TestDoubleReadWithoutDataRepeat)
{
    std::thread read_thread([]()
                            {
                                int size = 1024 * 1024; // 1 MB
                                uint8_t *payload = new uint8_t[size];
                                for (int i = 0; i < size; i++)
                                {
                                    payload[i] = i % 100;
                                }

                                TCPLink client("127.0.0.1", 20011, -1);
                                ASSERT_TRUE(client.writeWithAck(payload, size, 123.45));
                            });

    TCPLink server(20011, -1);

    auto [data, timestamp] = server.readMessageWithAck();
    printf("message received\n");

     int rcv_size = 1024 * 1024;

    ASSERT_EQ(data.size(), rcv_size);
    ASSERT_FLOAT_EQ(timestamp, 123.45);
    for (int i = 0; i < rcv_size; i++)
    {
        if (data[i] != i % 100)
            FAIL();
    }

    ASSERT_FALSE(server.hasData());
    auto [data2, timestamp2] = server.readMessage();
    ASSERT_FLOAT_EQ(timestamp2, 0);
    ASSERT_EQ(data2.size(), 0);

        // exec_start();
    read_thread.join();
    // exec_finished("1MB data recv client->server");

}
