#include "../include/databridge.h"

DataBrigde::DataBrigde(const char *inHost, int inPort, const char *outHost, int outPort, float timeout)
    : Datalink(inHost, inPort)
{
    this->outLink = new Datalink(outHost, outPort, timeout);
}

DataBrigde::DataBrigde(int inPort, const char *outHost, int outPort, float timeout)
    : Datalink(inPort)
{
    this->outLink = new Datalink(outHost, outPort, timeout);
}

DataBrigde::DataBrigde(int inPort, int outPort, float timeout)
    : Datalink(inPort)
{
    this->outLink = new Datalink(outPort, timeout);
}

DataBrigde::DataBrigde(const char *inHost, int inPort, int outPort, float timeout)
    : Datalink(inHost, inPort)
{
    this->outLink = new Datalink(outPort, timeout);
}

void DataBrigde::onDataReceived() {
    if (this->outLink->isReady())  {
        auto res = receiveData();
        this->outLink->sendData(res->data.get(), res->size);
    }
}

DataBrigde::~DataBrigde() {
    delete this->outLink;
}
