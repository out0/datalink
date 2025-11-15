#include <gtest/gtest.h>
#include <stdlib.h>
#include "../include/datalink.h"
#include <iostream>
#include "cpp_test_utils.h"


TEST(DataLinkConnectionTest, TestWrongConnection)
{
    auto client = new DatalinkConnection(0.5);
    ASSERT_FALSE(client->openConnection("127.0.0.1", 20000));
    delete client;
}

TEST(DataLinkConnectionTest, TestSimpleConnection)
{
    auto server = new DatalinkConnection(0.5);
    auto client = new DatalinkConnection(0.5);

    server->bindConnection(20000);
    auto p = parallelExecute([&]()
                             { server->socketListen(); });

    ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                             { return client->openConnection("127.0.0.1", 20000); },
                                             5000));
    parallelWaitFinish(p);

    server->closeConnection();
    client->closeConnection();

    ASSERT_FALSE(server->isConnected());
    ASSERT_FALSE(client->isConnected());

    delete server;
    delete client;
}
TEST(DataLinkConnectionTest, TestTimeout)
{
    auto server = new DatalinkConnection(0.5);
    auto client = new DatalinkConnection(0.5);

    server->bindConnection(20000);
    auto p = parallelExecute([&]()
                             { server->socketListen(); });

    ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                             { return client->openConnection("127.0.0.1", 20000); },
                                             500));
    parallelWaitFinish(p);

    ASSERT_FALSE(server->checkTimeout());
    // ensure timeout
    wait_ms(600);

    ASSERT_TRUE(server->checkTimeout());

    server->closeConnection();
    client->closeConnection();

    delete server;
    delete client;
}

TEST(DataLinkConnectionTest, TestTimeoutReset)
{
    auto server = new DatalinkConnection(0.5);
    auto client = new DatalinkConnection(0.5);


    server->bindConnection(20000);
    auto p = parallelExecute([&]()
                             { server->socketListen(); });

    ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                             { return client->openConnection("127.0.0.1", 20000); },
                                             500));
    parallelWaitFinish(p);

    ASSERT_FALSE(server->checkTimeout());

    for (int i = 0; i < 3; i++)
    {
        wait_ms(300);
        server->rstTimeout();
    }

    ASSERT_FALSE(server->checkTimeout());

    server->closeConnection();
    client->closeConnection();

    delete server;
    delete client;
}

TEST(DataLinkConnectionTest, TestDataSendServer2Client)
{
    auto server = new DatalinkConnection(0.5);
    auto client = new DatalinkConnection(0.5);

    server->bindConnection(20000);
    auto p = parallelExecute([&]()
                             { server->socketListen(); });

    ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                             { return client->openConnection("127.0.0.1", 20000); },
                                             500));
    parallelWaitFinish(p);

    char buffer[] = {0x11, 0x12, 0x13, 0x14, 0x15, 0x0};

    server->writeToSocket(buffer, sizeof(char) * 5);

    char outBuffer[]{0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    wait_ms(10);
    client->readFromSocket(outBuffer, sizeof(char) * 5);

    ASSERT_STREQ(buffer, outBuffer);

    server->closeConnection();
    client->closeConnection();

    delete server;
    delete client;
}

TEST(DataLinkConnectionTest, TestDataSendClient2Server)
{
    auto server = new DatalinkConnection(0.5);
    auto client = new DatalinkConnection(0.5);

    server->bindConnection(20000);
    auto p = parallelExecute([&]()
                             { server->socketListen(); });

    ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                             { return client->openConnection("127.0.0.1", 20000); },
                                             500));

    parallelWaitFinish(p);

    char buffer[] = {0x11, 0x12, 0x13, 0x14, 0x15, 0x0};

    client->writeToSocket(buffer, sizeof(char) * 5);

    char outBuffer[]{0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    wait_ms(10);
    server->readFromSocket(outBuffer, sizeof(char) * 5);

    ASSERT_STREQ(buffer, outBuffer);
    server->closeConnection();
    client->closeConnection();

    delete server;
    delete client;
}

TEST(DataLinkConnectionTest, TestDataSendBiDirectional)
{
    auto server = new DatalinkConnection(0.5);
    auto client = new DatalinkConnection(0.5);

    server->bindConnection(20000);
    auto p = parallelExecute([&]()
                             { server->socketListen(); });

    ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                             { return client->openConnection("127.0.0.1", 20000); },
                                             500));
    parallelWaitFinish(p);

    char buffer1[] = {0x11, 0x12, 0x13, 0x14, 0x15, 0x0};
    char buffer2[] = {0x16, 0x17, 0x18, 0x19, 0x20, 0x0};

    client->writeToSocket(buffer1, sizeof(char) * 5);
    server->writeToSocket(buffer2, sizeof(char) * 5);

    char outBuffer[]{0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    wait_ms(10);

    server->readFromSocket(outBuffer, sizeof(char) * 5);
    ASSERT_STREQ(buffer1, outBuffer);

    client->readFromSocket(outBuffer, sizeof(char) * 5);
    ASSERT_STREQ(buffer2, outBuffer);

    server->closeConnection();
    client->closeConnection();

    delete server;
    delete client;
}