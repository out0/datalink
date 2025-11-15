#include <gtest/gtest.h>
#include <stdlib.h>
#include "../../include/datalink.h"
#include "cpp_test_utils.h"

TEST(DataLinkTest, TestSendMessageServer2Client)
{
    char msg[] = {0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x00};
    auto srv = std::make_shared<Datalink>(20000, -1);
    auto cli = std::make_shared<Datalink>("127.0.0.1", 20000, -1);

    ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                             { return srv->isReady(); },
                                             500));
    ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                             { return cli->isReady(); },
                                             500));

    ASSERT_TRUE(srv->sendData(msg, 10));

    auto result = cli->receiveData(2);

    ASSERT_FALSE(result == nullptr);

    ASSERT_TRUE(result->valid);
    ASSERT_EQ(result->size, 10);
    ASSERT_STREQ(result->data.get(), msg);

    srv.reset();
    cli.reset();
}

TEST(DataLinkTest, TestSendMessageServer2ClientFloat)
{
    int size = 1000000;
    float *msg = new float[size];

    for (int i = 0; i < size; i++)
        msg[i] = i * 1.001;

    auto srv = std::make_shared<Datalink>(20000, -1);
    auto cli = std::make_shared<Datalink>("127.0.0.1", 20000, -1);

    ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                             { return srv->isReady(); },
                                             500));
    ASSERT_TRUE(executeUntilSuccessOrTimeout([&]
                                             { return cli->isReady(); },
                                             500));

    ASSERT_TRUE(srv->sendData(msg, size));

    auto result = cli->receiveDataF(2);

    ASSERT_FALSE(result == nullptr);

    ASSERT_TRUE(result->valid);
    ASSERT_EQ(result->size, size);

    for (int i = 0; i < size; i++)
       if (result->data[i] != (float)(i * 1.001)) {
            printf("data is corrupted at %d: expected %f obtained %f\n", i, i*1.001, result->data[i]);
            FAIL();
       }

    srv.reset();
    cli.reset();
}
