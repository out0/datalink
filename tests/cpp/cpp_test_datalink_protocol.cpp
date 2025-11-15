#include <gtest/gtest.h>
#include <stdlib.h>
#include "../../include/datalink.h"
#include "cpp_test_utils.h"


TEST(DataLinkProtocolTest, TestSendMessageServer2Client)
{
    char msg[] = {0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x00};
    auto srv = std::make_shared<DatalinkProtocol>(std::make_shared<DatalinkConnection>(0.5));
    auto cli = std::make_shared<DatalinkProtocol>(std::make_shared<DatalinkConnection>(0.5));

    srv->getConnection()->bindConnection(20000);
    auto p = parallelExecute([&]
                             { srv->getConnection()->socketListen(); });
    ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                             { return cli->getConnection()->openConnection("127.0.0.1", 20000); },
                                             500));
    parallelWaitFinish(p);

    ASSERT_TRUE(srv->writeMessage(msg, 10));
    
    ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                             { return cli->readMessage(); },
                                             500));
    auto result = cli->releaseBuffer();
    ASSERT_TRUE(result->valid);
    ASSERT_EQ(result->size, 10);
    ASSERT_STREQ(result->data.get(), msg);

    srv->getConnection()->closeConnection();
    cli->getConnection()->closeConnection();

    srv.reset();
    cli.reset();
}


TEST(DataLinkProtocolTest, TestSendMessageClient2Server)
{
    char msg[] = {0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x00};
    auto srv = std::make_shared<DatalinkProtocol>(std::make_shared<DatalinkConnection>(0.5));
    auto cli = std::make_shared<DatalinkProtocol>(std::make_shared<DatalinkConnection>(0.5));

    srv->getConnection()->bindConnection(20000);
    auto p = parallelExecute([&]
                             { srv->getConnection()->socketListen(); });
    ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                             { return cli->getConnection()->openConnection("127.0.0.1", 20000); },
                                             500));
    parallelWaitFinish(p);

    ASSERT_TRUE(cli->writeMessage(msg, 10));
    ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                             { return srv->readMessage(); },
                                             500));
    auto result = srv->releaseBuffer();
    ASSERT_TRUE(result->valid);
    ASSERT_EQ(result->size, 10);
    ASSERT_STREQ(result->data.get(), msg);

    srv->getConnection()->closeConnection();
    cli->getConnection()->closeConnection();

    srv.reset();
    cli.reset();
}
TEST(DataLinkProtocolTest, TestSendMessageBidirectional)
{
    char msg[] = {0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x00};
    auto srv = std::make_shared<DatalinkProtocol>(std::make_shared<DatalinkConnection>(0.5));
    auto cli = std::make_shared<DatalinkProtocol>(std::make_shared<DatalinkConnection>(0.5));


    srv->getConnection()->bindConnection(20000);
    auto p = parallelExecute([&]
                             { srv->getConnection()->socketListen(); });
    ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                             { return cli->getConnection()->openConnection("127.0.0.1", 20000); },
                                             500));
    parallelWaitFinish(p);

    ASSERT_TRUE(cli->writeMessage(msg, 10));
    ASSERT_TRUE(srv->writeMessage(msg, 10));

    ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                             { return cli->readMessage(); },
                                             500));

    ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                             { return srv->readMessage(); },
                                             500));

    auto result = srv->releaseBuffer();
    ASSERT_TRUE(result->valid);
    ASSERT_EQ(result->size, 10);
    ASSERT_STREQ(result->data.get(), msg);

    auto result2 = cli->releaseBuffer();
    ASSERT_TRUE(result2->valid);
    ASSERT_EQ(result2->size, 10);
    ASSERT_STREQ(result2->data.get(), msg);

    srv->getConnection()->closeConnection();
    cli->getConnection()->closeConnection();

    srv.reset();
    cli.reset();
}

TEST(DataLinkProtocolTest, TestSendMessageAfterBufferRelease)
{
    char msg[] = {0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x00};
    auto srv = std::make_shared<DatalinkProtocol>(std::make_shared<DatalinkConnection>(0.5));
    auto cli = std::make_shared<DatalinkProtocol>(std::make_shared<DatalinkConnection>(0.5));


    srv->getConnection()->bindConnection(20000);
    auto p = parallelExecute([&]
                             { srv->getConnection()->socketListen(); });
    ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                             { return cli->getConnection()->openConnection("127.0.0.1", 20000); },
                                             500));
    parallelWaitFinish(p);

    for (int i = 0; i < 5; i++)
    {
        ASSERT_TRUE(cli->writeMessage(msg, 10));
        ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                                 { return srv->readMessage(); },
                                                 500));
        auto result = srv->releaseBuffer();
        ASSERT_TRUE(result->valid);
        ASSERT_EQ(result->size, 10);
        ASSERT_STREQ(result->data.get(), msg);
    }

    srv->getConnection()->closeConnection();
    cli->getConnection()->closeConnection();

    srv.reset();
    cli.reset();
}
