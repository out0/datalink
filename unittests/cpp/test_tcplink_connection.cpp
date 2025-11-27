#include <gtest/gtest.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "../../include/tcplink.h"


TEST(TCPLinkConnection, TestConnectionNoTimeoutSuccess)
{
    TCPLink server(20001, -1);
    TCPLink client("127.0.0.1", 20001, -1);

    int max_loops = 10;
    while (max_loops >= 0) {
        if (!server.isReady()) {
            printf("server not ready\n");
        }
        if (!client.isReady()) {
            printf("client not ready\n");
        }
        if (server.isReady() && client.isReady()) {
            printf("client and server are ready\n");
            SUCCEED();
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        max_loops--;
    }

    FAIL();
}


TEST(TCPLinkConnection, TestConnectionTimeoutSuccess)
{
    TCPLink server(20000, 100);
    TCPLink client("localhost", 20000, 100);

    int max_loops = 10;
    while (max_loops >= 0) {
        if (!server.isReady()) {
            printf("server not ready\n");
        }
        if (!client.isReady()) {
            printf("client not ready\n");
        }
        if (server.isReady() && client.isReady()) {
            printf("client and server are ready\n");
            SUCCEED();
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        max_loops--;
    }

    FAIL();
}


TEST(TCPLinkConnection, TestConnectionNoTimeoutFail)
{
    TCPLink server(20003, -1);
    TCPLink client("localhost", 20004, -1);

    int max_loops = 10;
    while (max_loops >= 0) {
        if (server.isReady() && client.isReady()) {
            printf("client and server are ready, but they should not be\n");
            FAIL();
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        max_loops--;
    }

    SUCCEED();
}


TEST(TCPLinkConnection, TestConnectionTimeoutFail)
{
    TCPLink server(20004, 100);
    TCPLink client("localhost", 20005, 100);

    int max_loops = 10;
    while (max_loops >= 0) {
        if (server.isReady() && client.isReady()) {
            printf("client and server are ready, but they should not be\n");
            FAIL();
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        max_loops--;
    }

    SUCCEED();
}