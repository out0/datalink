#ifndef _DATA_LINK_H
#define _DATA_LINK_H

/*

Datalink protocol

Message package:
------------------
signature: 0x32 0x32 0x32 0x32 0x32 (long size - 8 bytes) 0x33 0x33 0x33 0x33 0x33
data: ,,,,
message_end: 0x34 0x34 0x34 0x34 0x34 0x35 0x35 0x35 0x35 0x35

writeData() sends data through the socked promptly
readData() reads last received data, when called, puts internal threading into ignoring new data

*/

#include <string>
#include <thread>
#include <mutex>
#include <memory>
#include <vector>

typedef union
{
    float fval;
    uint8_t bval[sizeof(float)];
} floatp;

typedef union
{
    double fval;
    uint8_t bval[sizeof(double)];
} doublep;


typedef union
{
    long val;
    uint8_t bval[sizeof(long)];
} longp;

typedef union
{
    int32_t val;
    uint8_t bval[sizeof(int32_t)];
} intp;

typedef union uint8
{
    uint8_t val;
    uint8_t bval;
} uint8;

typedef union int8p
{
    int8_t val;
    uint8_t bval;
} int8p;


class Datalink
{

public:
    Datalink(const char *server, int port, float no_data_timeout_s = -1);
    Datalink(int port, float no_data_timeout_s = -1);
    ~Datalink();

    bool isReady();
    // bool hasData();
    

    // bool sendData(char *data, long data_size);
    // bool sendData(float *data, long data_size);
    // bool sendData(uint8_t *data, long dataSize);


};


#endif
