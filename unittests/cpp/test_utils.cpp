#include <gtest/gtest.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "../../include/tcplink.h"

extern char *data_encode(int *data, long size);
extern int *data_decode_int(char *data, long size);


TEST(DatalinkUtils, TestEncodeDecodeInteger)
{
    long size = 1024*1024;
    int *data = new int[size];

    for (int i = 0; i < size; i++) {
        data[i] = i;
    }

    char *encoded_data = data_encode(data, size);

    int *decoded_data = data_decode_int(encoded_data, size);

    for (int i = 0; i < size; i++) {
        if (decoded_data[i] != data[i]) {
            printf ("Integer encode/decode failing at %d\n", i);
            FAIL();
        }
    }

    delete []encoded_data;
    delete []decoded_data;
}