#include <gtest/gtest.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "../../include/tcplink.h"

extern char *data_encode(int8_t *data, long size);
extern char *data_encode(int *data, long size);
extern char *data_encode(long *data, long size);
extern char *data_encode(float *data, long size);
extern char *data_encode(double *data, long size);
extern int *data_decode_int(char *data, long size);
extern int8_t *data_decode_int8(char *data, long size);
extern long *data_decode_long(char *data, long size);
extern float *data_decode_float(char *data, long size);
extern double *data_decode_double(char *data, long size);


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

TEST(DatalinkUtils, TestEncodeDecodeLong)
{
    long size = 1024*1024;
    long *data = new long[size];

    for (int i = 0; i < size; i++) {
        data[i] = i;
    }

    char *encoded_data = data_encode(data, size);

    long *decoded_data = data_decode_long(encoded_data, size);

    for (int i = 0; i < size; i++) {
        if (decoded_data[i] != data[i]) {
            printf ("Long encode/decode failing at %d\n", i);
            FAIL();
        }
    }

    delete []encoded_data;
    delete []decoded_data;
}

TEST(DatalinkUtils, TestEncodeDecodeFloat)
{
    long size = 1024*1024;
    float *data = new float[size];

    for (int i = 0; i < size; i++) {
        data[i] = 1.0 + i;
    }

    char *encoded_data = data_encode(data, size);

    float *decoded_data = data_decode_float(encoded_data, size);

    for (int i = 0; i < size; i++) {
        if (decoded_data[i] != data[i]) {
            printf ("Float encode/decode failing at %d\n", i);
            FAIL();
        }
    }

    delete []encoded_data;
    delete []decoded_data;
}

TEST(DatalinkUtils, TestEncodeDecodeDouble)
{
    long size = 1024*1024;
    double *data = new double[size];

    for (int i = 0; i < size; i++) {
        data[i] = 1.0 + i;
    }

    char *encoded_data = data_encode(data, size);

    double *decoded_data = data_decode_double(encoded_data, size);

    for (int i = 0; i < size; i++) {
        if (decoded_data[i] != data[i]) {
            printf ("Double encode/decode failing at %d\n", i);
            FAIL();
        }
    }

    delete []encoded_data;
    delete []decoded_data;
}

TEST(DatalinkUtils, TestEncodeDecodeInt8)
{
    long size = 1024*1024;
    int8_t *data = new int8_t[size];

    for (int i = 0; i < size; i++) {
        data[i] = i % 255;
    }

    char *encoded_data = data_encode(data, size);

    int8_t *decoded_data = data_decode_int8(encoded_data, size);

    for (int i = 0; i < size; i++) {
        if (decoded_data[i] != data[i]) {
            printf ("Int8 encode/decode failing at %d\n", i);
            FAIL();
        }
    }

    delete []encoded_data;
    delete []decoded_data;
}