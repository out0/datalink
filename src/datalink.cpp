#include <iostream>
#include <sys/socket.h> // Include for socket APIs
#include <netinet/in.h> // Include for internet protocols
#include <cstring>      // Include for string operations
#include <unistd.h>     // Include for POSIX operating system API
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include "../include/datalink.h"

//#define DEBUG 1

bool Datalink::receiveWaitForDataTimeout(double start, double timeout_s)
{
    if (timeout_s <= 0)
        return false;
    return DatalinkCommon::timeNow() - start > timeout_s;
}

Datalink::Datalink(const char *host, int port, float no_data_timeout_s)
{
    this->conn = std::make_shared<DatalinkConnection>(no_data_timeout_s);
    this->protocol = std::make_unique<DatalinkProtocol>(this->conn);
    this->host = strdup(host);
    this->port = port;
    this->run = true;
    this->state = CONNECTION_CLOSED;
    this->linkRunThread = std::make_unique<std::thread>(&Datalink::linkRun, this);
}
Datalink::Datalink(int port, float no_data_timeout_s)
{
    this->conn = std::make_shared<DatalinkConnection>(no_data_timeout_s);
    this->protocol = std::make_unique<DatalinkProtocol>(this->conn);
    this->host = nullptr;
    this->port = port;
    this->run = true;
    this->state = CONNECTION_CLOSED;
    this->linkRunThread = std::make_unique<std::thread>(&Datalink::linkRun, this);
}
Datalink::~Datalink()
{
    free(this->host);
    this->run = false;
    this->linkRunThread->join();
}

void Datalink::linkRun()
{
    while (this->run)
    {
        switch (state)
        {
        case CONNECTION_CLOSED:
            this->state = openLink();
            break;
        case WAIT_LISTEN:
            this->state = waitListen();
            break;
        case CONNECTION_OPENED:
            this->state = dataTransfer();
            break;
        default:
            this->state = CONNECTION_CLOSED;
            break;
        }
    }
}

int Datalink::openLink()
{
    if (host == nullptr)
    {
        if (this->conn->bindConnection(this->port))
            return WAIT_LISTEN;

        return CONNECTION_CLOSED;
    }

    if (this->conn->openConnection(this->host, this->port))
    {
        this->conn->rstTimeout();
        return CONNECTION_OPENED;
    }

    return CONNECTION_CLOSED;
}

int Datalink::waitListen()
{
    if (this->conn->socketListen())
    {
        this->conn->rstTimeout();
        return CONNECTION_OPENED;
    }
    return WAIT_LISTEN;
}

int Datalink::dataTransfer()
{
    if (conn->checkTimeout()) {
        return terminateSession();
    }

    if (this->protocol->readMessage())
        this->conn->rstTimeout();
    else
        DatalinkCommon::wait_ms(2);

    return CONNECTION_OPENED;
}

int Datalink::terminateSession()
{
#ifdef DEBUG
    printf ("terminate session called\n");
#endif
    if (state == CONNECTION_OPENED)
    {
        this->conn->closeConnection();

        if (host == nullptr)
            return CONNECTION_CLOSED;
    }
    return CONNECTION_CLOSED;
}

bool Datalink::hasData()
{
    return this->protocol->hasData();
}
bool Datalink::isReady()
{
#ifdef DEBUG
    printf("[datalink] isReady() called, state = %d\n", this->state);
#endif
    return this->state == CONNECTION_OPENED;
}

bool Datalink::sendData(char *data, long dataSize)
{
    return this->protocol->writeMessage(data, dataSize);
}
bool Datalink::sendData(float *data, long dataSize)
{
    return this->protocol->writeMessage(data, dataSize);
}
bool Datalink::sendData(uint8_t *data, long dataSize)
{
    return this->protocol->writeMessage(data, dataSize);
}
std::shared_ptr<DataLinkResult<char>> Datalink::receiveData(double timeout_s)
{
    double start = DatalinkCommon::timeNow();

    while (!receiveWaitForDataTimeout(start, timeout_s))
    {
        // if (!this->protocol->hasData()) {
        //     DatalinkCommon::wait_ms(1);
        //     continue;
        // }

        auto buff = this->protocol->releaseBuffer();

        if (buff != nullptr) {
            return buff;
        }

    }
    return nullptr;
}

std::shared_ptr<DataLinkResult<float>> Datalink::receiveDataF(double timeout_s)
{
    std::shared_ptr<DataLinkResult<char>> rawData = receiveData(timeout_s);
    if (rawData == nullptr)
        return nullptr;

    if (rawData->size % sizeof(float) != 0)
    {
        printf("[datalink] data mismatch when receiving float: %ld is not a multiple of float size %ld\n", rawData->size, sizeof(float));
        return nullptr;
    }

    char * buffer = rawData->data.get();
    auto res = std::make_shared<DataLinkResult<float>>();
    res->size = 0;
    res->valid = true;
    res->data = std::make_unique<float[]>(rawData->size / sizeof(float));

    floatp p;
    res->size = 0;
    for (long i = 0; i < rawData->size; ) {
        for (int k = 0; k < sizeof(float); k++, i++) {
            p.bval[k] = buffer[i];
        }
        res->data[res->size] = p.fval;
        res->size++;
    }

    return res;
}
