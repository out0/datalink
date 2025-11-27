#include <gtest/gtest.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "../../include/datalink.h"

class DatalinkDummy : protected Datalink
{
public:
    DatalinkDummy() : Datalink(1000, -1) {
        _link_ready = true;
    }

    void write_data(int fd, const char *payload, long size)
    {
        _connSockFd = fd;
        _write(payload, size);
    }
};

TEST(DataLinkReadWrite, TestWrite)
{
    DatalinkDummy dummy_link;

    int fd = open("test_data_dump.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    ASSERT_GE(fd, 0);

    const char *payload = "test payload";
    long size = (long)strlen(payload);

    dummy_link.write_data(fd, payload, size);

    close(fd);
}