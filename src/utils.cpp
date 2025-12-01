#include <chrono>
#include "../include/datalink.h"

timeval set_timeout_ms(double timeout_ms)
{
    timeval time;
    long s = static_cast<long>(timeout_ms / 1000);
    long us = static_cast<long>((timeout_ms - 1000 * s) * 1000);
    time.tv_sec = s;
    time.tv_usec = us;
    // printf ("set_timeout_ms: s = %ld, us=%ld\n", s, us);
    return time;
}

/*
uint8_t *data_encode(int *data, long size)
{
    uint8_t *p = new uint8_t[size * sizeof(int32_t)];
    intp val;

    for (long i = 0, pos = 0; i < size; i++)
    {
        val.val = data[i];
        for (int j = 0; j < sizeof(int32_t); j++)
        {
            p[pos] = val.bval[j];
            pos++;
        }
    }
    return p;
}
uint8_t *data_encode(long *data, long size)
{
    uint8_t *p = new uint8_t[size * sizeof(long)];
    longp val;

    for (long i = 0, pos = 0; i < size; i++)
    {
        val.val = data[i];
        for (int j = 0; j < sizeof(long); j++)
        {
            p[pos] = val.bval[j];
            pos++;
        }
    }
    return p;
}
uint8_t *data_encode(float *data, long size)
{
    uint8_t *p = new uint8_t[size * sizeof(float)];
    floatp val;

    for (long i = 0, pos = 0; i < size; i++)
    {
        val.fval = data[i];
        for (int j = 0; j < sizeof(float); j++)
        {
            p[pos] = val.bval[j];
            pos++;
        }
    }
    return p;
}
uint8_t *data_encode(double *data, long size)
{
    uint8_t *p = new uint8_t[size * sizeof(double)];
    doublep val;

    for (long i = 0, pos = 0; i < size; i++)
    {
        val.fval = data[i];
        for (int j = 0; j < sizeof(double); j++)
        {
            p[pos] = val.bval[j];
            pos++;
        }
    }
    return p;
}
uint8_t *data_encode(int8_t *data, long size)
{
    uint8_t *p = new uint8_t[size * sizeof(int8_t)];
    int8p val;

    for (long i = 0; i < size; i++)
    {
        val.val = data[i];
        p[i] = val.bval;
    }
    return p;
}
int *data_decode_int(uint8_t *data, long size)
{
    int *p = new int[size];
    intp val;

    for (long i = 0, pos = 0; i < size; i++)
    {
        for (int j = 0; j < sizeof(int32_t); j++)
        {
            val.bval[j] = data[pos];
            pos++;
        }
        p[i] = val.val;
    }
    return p;
}
int8_t *data_decode_int8(uint8_t *data, long size)
{
    int8_t *p = new int8_t[size];
    int8p val;

    for (long i = 0; i < size; i++)
    {
        val.bval = data[i];
        p[i] = val.val;
    }
    return p;
}
long *data_decode_long(uint8_t *data, long size)
{
    long *p = new long[size];
    longp val;

    for (long i = 0, pos = 0; i < size; i++)
    {
        for (int j = 0; j < sizeof(long); j++)
        {
            val.bval[j] = data[pos];
            pos++;
        }
        p[i] = val.val;
    }
    return p;
}
float *data_decode_float(uint8_t *data, long size)
{
    float *p = new float[size];
    floatp val;

    for (long i = 0, pos = 0; i < size; i++)
    {
        for (int j = 0; j < sizeof(float); j++)
        {
            val.bval[j] = data[pos];
            pos++;
        }
        p[i] = val.fval;
    }
    return p;
}
double *data_decode_double(uint8_t *data, long size)
{
    double *p = new double[size];
    doublep val;

    for (long i = 0, pos = 0; i < size; i++)
    {
        for (int j = 0; j < sizeof(double); j++)
        {
            val.bval[j] = data[pos];
            pos++;
        }
        p[i] = val.fval;
    }
    return p;
}
*/