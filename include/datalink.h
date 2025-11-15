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
#include <atomic>
#include <mutex>
#include <memory>

#define PROTOCOL_START_MESSAGE_SIZE (10 + sizeof(long)) * sizeof(char)
#define PROTOCOL_END_MESSAGE_SIZE 10 * sizeof(char)

template <typename T>
class DataLinkResult
{
public:
    std::unique_ptr<T[]> data;
    long size;
    bool valid;
};

enum DataLinkState
{
    CONNECTION_CLOSED = 0,
    WAIT_LISTEN = 1,
    CONNECTION_OPENED = 3
};

class DatalinkCommon
{
public:
    static double timeNow()
    {
        return std::chrono::duration_cast<std::chrono::duration<double>>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
    }
    static int min(int a, int b)
    {
        return a < b ? a : b;
    }
    static void wait_ms(int ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
};

class DatalinkConnection
{
    int sock;
    long timeoutStart;
    float noDataTimeout_s;
    bool isOpened;

private:
    bool checkConnectionLost(int socketResult, int errorCode);

public:
    DatalinkConnection(float no_data_timeout_s);
    ~DatalinkConnection();

    void rstTimeout();
    bool checkTimeout();

    bool bindConnection(int port);
    bool openConnection(const char *host, int port);
    bool socketListen();
    void closeConnection();
    bool isConnected();

    bool readFromSocket(char *buffer, long expectedSize);
    bool writeToSocket(char *buffer, long expectedSize);
};
class DatalinkProtocol
{
    std::shared_ptr<DatalinkConnection> conn;
    char messageStart[PROTOCOL_START_MESSAGE_SIZE + sizeof(char)];
    const char messageEnd[PROTOCOL_END_MESSAGE_SIZE + sizeof(char)] = {0x22, 0x22, 0x22, 0x22, 0x22, 0x23, 0x23, 0x23, 0x23, 0x23, 0x00};
    std::unique_ptr<DataLinkResult<char>> lastBuffer;
    std::unique_ptr<char[]> protocolBufferRcv;
    std::atomic<bool> bufferInRelease;
    std::mutex bufferMutex;

    bool checkRepeatByte(char *buffer, int startPos, int count, int byte);
    bool updateBuffer(long size);
    long checkMessageStart();
    bool checkMessageEnd();
    bool writeMessageStart(long messageSize);
    bool writeMessageEnd();

public:
    DatalinkProtocol(std::shared_ptr<DatalinkConnection> conn);
    bool readMessage();
    bool writeMessage(char *data, long size);
    bool writeMessage(float *data, long size);
    bool writeMessage(uint8_t *data, long dataSize);
    std::shared_ptr<DataLinkResult<char>> releaseBuffer();
    std::shared_ptr<DatalinkConnection> getConnection();
    bool hasData();
};

class Datalink
{
private:
    char * host;
    int port;
    bool run;
    int state;
    std::unique_ptr<std::thread> linkRunThread;
    std::shared_ptr<DatalinkConnection> conn;
    std::unique_ptr<DatalinkProtocol> protocol;

    bool receiveWaitForDataTimeout(double start, double timeout_s);
    void linkRun();
    int openLink();
    int waitListen();
    int dataTransfer();
    int terminateSession();

protected:
    virtual void onDataReceived() {}

public:
    Datalink(const char *server, int port, float no_data_timeout_s = -1);
    Datalink(int port, float no_data_timeout_s = -1);
    ~Datalink();

    bool hasData();
    bool isReady();

    bool sendData(char *data, long data_size);
    bool sendData(float *data, long data_size);
    bool sendData(uint8_t *data, long dataSize);

    std::shared_ptr<DataLinkResult<char>> receiveData(double timeout_s = -1);
    std::shared_ptr<DataLinkResult<float>> receiveDataF(double timeout_s = -1);
};

typedef union
{
    float fval;
    char bval[sizeof(float)];
} floatp;

typedef union
{
    long val;
    char bval[sizeof(long)];
} longp;

typedef union uint8
{
    uint8_t val;
    char bval;
} uint8;


#endif
