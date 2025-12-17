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

#include <vector>
#include <tuple>
#include <memory>
#include "serializer.h"

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
    virtual bool isReady() = 0;
    virtual bool write(const uint8_t *payload, long payload_size, double timestamp) = 0;
    virtual bool writeKeepAlive() = 0;
    virtual bool hasData() = 0;

    virtual std::tuple<std::vector<uint8_t>, double> readMessage() = 0;
    virtual long readMessageSize() = 0;
    virtual long readMessageToBuffer(uint8_t *buffer, long size, double *timestamp) = 0;
    virtual void clearBuffer() = 0;

    virtual float timeout_ms() = 0;

    static std::shared_ptr<Datalink> TcpServer(int port, float no_data_timeout_ms = -1, int max_incomming_queued_messages = -1);
    static std::shared_ptr<Datalink> TcpClient(const char *host, int port, float no_data_timeout_ms = -1, int max_incomming_queued_messages = -1);

    virtual void setForwardMode() = 0;

    bool writeWithAck(uint8_t *payload, long size, double timestamp = 1);
    std::tuple<std::vector<uint8_t>, double> readMessageWithAck();

    // template <typename T>
    // bool sendObject(T obj, double timestamp = 1)
    // {
    //     Serializer s(sizeof(T));
    //     s.write(obj, sizeof(T));

    //     auto payload = s.get();
    //     printf("send: [");
    //     for (int i = 0; i < payload.size(); i++)
    //         printf(" %d", payload[i]);
    //     printf(" ]\n");

    //     return writeWithAck(&payload[0], payload.size());
    // }

    // template <typename T>
    // std::tuple<T *, double> readObject()
    // {
    //     auto [payload, timestamp] = readMessageWithAck();
    //     if (timestamp < 0 || payload.size() == 0)
    //         return {nullptr, timestamp};

    //     Serializer s(payload);

    //     printf("rcvd: [");
    //     for (int i = 0; i < payload.size(); i++)
    //         printf(" %d", payload[i]);
    //     printf(" ]\n");

        
    //     return {s.read_ptr<T>(), timestamp};
    // }

    // needed to ensure that any child destructors are going to be called!
    virtual ~Datalink() {}
};

#endif
