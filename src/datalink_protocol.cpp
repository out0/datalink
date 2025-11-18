#include <cstring>
#include "../include/datalink.h"
//#define DEBUG 1

// void printBuffer(char *buffer, long size) {
//     printf("[");
//     for (long i = 0; i < size; i++)
//         printf(" %d", buffer[i]);
//     printf(" ]\n");
// }

bool DatalinkProtocol::checkRepeatByte(char *buffer, int startPos, int count, int byte)
{
    for (int i = startPos; i < startPos + count; i++)
        if (buffer[i] != byte)
            return false;
    return true;
}

DatalinkProtocol::DatalinkProtocol(std::shared_ptr<DatalinkConnection> conn)
{
    this->conn = conn;
    this->bufferInRelease = false;

    protocolBufferRcv = std::make_unique<char[]>(PROTOCOL_START_MESSAGE_SIZE);
    for (int i = 0; i < 5; i++)
    {
        messageStart[i] = 0x20;
        messageStart[PROTOCOL_START_MESSAGE_SIZE - i - 1] = 0x21;
    }
}

long DatalinkProtocol::checkMessageStart()
{
    char *buffer = protocolBufferRcv.get();

    if (!this->conn->readFromSocket(buffer, PROTOCOL_START_MESSAGE_SIZE))
    {
#ifdef DEBUG
        printf("[datalink] checkMessageRcvStart failed\n");
#endif
        return -1;
    }

    if (!checkRepeatByte(buffer, 0, 5, 32))
        return false;

    longp res;
    for (int i = 5; i < 5 + sizeof(long); i++)
        res.bval[i - 5] = buffer[i];

    if (!checkRepeatByte(buffer, 5 + sizeof(long), 5, 33))
        return false;

#ifdef DEBUG
    printf("[datalink] message start detected with %ld bytes\n", res.val);
#endif

    return res.val;
}
bool DatalinkProtocol::checkMessageEnd()
{
    char *buffer = protocolBufferRcv.get();

    if (!this->conn->readFromSocket(buffer, PROTOCOL_END_MESSAGE_SIZE))
        return false;

    if (!checkRepeatByte(buffer, 0, 5, 34))
        return false;

    if (!checkRepeatByte(buffer, 5, 5, 35))
        return false;

#ifdef DEBUG
    printf("[datalink] message end successfully detected\n");
#endif

    return true;
}

bool DatalinkProtocol::writeMessageStart(long messageSize)
{
    longp size;
    size.val = messageSize;

    for (int i = 5; i < 5 + sizeof(long); i++)
        messageStart[i] = size.bval[i - 5];

    return conn->writeToSocket(messageStart, PROTOCOL_START_MESSAGE_SIZE);
}
bool DatalinkProtocol::writeMessageEnd()
{
    return conn->writeToSocket((char *)&messageEnd, PROTOCOL_END_MESSAGE_SIZE);
}

bool DatalinkProtocol::updateBuffer(long size)
{
    if (this->lastBuffer == nullptr)
    {
        this->lastBuffer = std::make_unique<DataLinkResult<char>>();
        if (this->lastBuffer == nullptr)
            return false;

        this->lastBuffer->size = 0;
    }

    if (this->lastBuffer->size != size)
    {
#ifdef DEBUG        
        printf ("updateBuffer() modifying the data buffer from %ld to %ld\n", this->lastBuffer->size, size);
#endif        
        this->lastBuffer->data = std::make_unique<char[]>(size + 1);
        this->lastBuffer->size = size;
    }

    return this->lastBuffer->data != nullptr;
}

bool DatalinkProtocol::readMessage()
{
    if (!conn->isConnected())
        return false;

    if (this->bufferInRelease) {
        return false;
    }

    if (!bufferMutex.try_lock())
        return false;

    long size = checkMessageStart();
    if (size <= 0)
    {
        bufferMutex.unlock();
        return false;
    }

    conn->rstTimeout();

    if (lastBuffer != nullptr)
        lastBuffer->valid = false;

    if (!updateBuffer(size))
    {
        bufferMutex.unlock();
        return false;
    }

    char *dataBufferPtr = this->lastBuffer->data.get();

    if (!conn->readFromSocket(dataBufferPtr, size))
    {
        bufferMutex.unlock();
        return false;
    }
    dataBufferPtr[size] = 0x0;

    if (checkMessageEnd())
    {
        conn->rstTimeout();
        lastBuffer->valid = true;
    }

    bufferMutex.unlock();
    return lastBuffer->valid;
}
bool DatalinkProtocol::writeMessage(char *data, long size)
{
    if (!conn->isConnected())
        return false;

    if (!writeMessageStart(size))
        return false;

    conn->rstTimeout();

    if (!conn->writeToSocket(data, size))
        return false;

    bool p = writeMessageEnd();
    if (p)
        conn->rstTimeout();
    return p;
}
bool DatalinkProtocol::writeMessage(float *data, long size)
{
    if (!conn->isConnected())
        return false;

    char *tempBuffer = new char[sizeof(float) * size];
    if (tempBuffer == nullptr)
        return false;

    floatp p;
    for (long i = 0, j = 0; i < size; i++)
    {
        p.fval = data[i];
        for (int k = 0; k < sizeof(float); k++)
        {
            tempBuffer[j++] = p.bval[k];
        }
    }

    //printBuffer(tempBuffer, sizeof(float) * size);

    bool res = writeMessage(tempBuffer, sizeof(float) * size);
    delete tempBuffer;
    return res;
}

bool DatalinkProtocol::writeMessage(uint8_t *data, long size)
{
   if (!conn->isConnected())
        return false;

    char *tempBuffer = new char[sizeof(uint8_t) * size];
    if (tempBuffer == nullptr)
        return false;

    uint8 p;
    for (long i = 0, j = 0; i < size; i++)
    {
        p.val = data[i];
        for (int k = 0; k < sizeof(uint8_t); k++)
        {
            tempBuffer[j++] = p.bval;
        }
    }

    //printBuffer(tempBuffer, sizeof(float) * size);

    bool res = writeMessage(tempBuffer, sizeof(uint8_t) * size);
    delete tempBuffer;
    return res;
}
std::shared_ptr<DataLinkResult<char>> DatalinkProtocol::releaseBuffer()
{
    this->bufferInRelease = true;
    bufferMutex.lock();

    if (this->lastBuffer == nullptr)
    {
        this->bufferInRelease = false;
        bufferMutex.unlock();
        return nullptr;
    }

    if (!this->lastBuffer->valid)
    {
        this->bufferInRelease = false;
        bufferMutex.unlock();
        return nullptr;
    }

    DataLinkResult<char> *p = new DataLinkResult<char>();
    p->size = lastBuffer->size;
    p->valid = true;
    p->data = std::make_unique<char[]>(lastBuffer->size + 1);
    memcpy(p->data.get(), lastBuffer->data.get(), p->size);

    this->bufferInRelease = false;
    this->lastBuffer->valid = false;
    bufferMutex.unlock();
    return std::shared_ptr<DataLinkResult<char>>(p);
}
std::shared_ptr<DatalinkConnection> DatalinkProtocol::getConnection()
{
    return this->conn;
}

bool DatalinkProtocol::hasData()
{
    return this->lastBuffer != nullptr && this->lastBuffer->valid;
}