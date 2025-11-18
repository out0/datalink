#include "../include/databridge.h"


DataBrigde::DataBrigde(const char *inHost, int inPort, const char *outHost, int outPort, float timeout)
{
    this->inLink = new Datalink(inHost, inPort, timeout);
    this->outLink = new Datalink(outHost, outPort, timeout);
    this->linkRunThread = std::make_unique<std::thread>(&DataBrigde::linkRun, this);
}

DataBrigde::DataBrigde(int inPort, const char *outHost, int outPort, float timeout)
{
    this->inLink = new Datalink(inPort, timeout);
    this->outLink = new Datalink(outHost, outPort, timeout);
    this->linkRunThread = std::make_unique<std::thread>(&DataBrigde::linkRun, this);
}

DataBrigde::DataBrigde(int inPort, int outPort, float timeout)
{
    this->inLink = new Datalink(inPort, timeout);
    this->outLink = new Datalink(outPort, timeout);
    this->linkRunThread = std::make_unique<std::thread>(&DataBrigde::linkRun, this);
}

DataBrigde::DataBrigde(const char *inHost, int inPort, int outPort, float timeout)
{
    this->inLink = new Datalink(inHost, inPort, timeout);
    this->outLink = new Datalink(outPort, timeout);
    this->linkRunThread = std::make_unique<std::thread>(&DataBrigde::linkRun, this);
}



void DataBrigde::linkRun()
{
    while (this->run)
    {
        if (this->inLink->isReady() && this->inLink->hasData()) {
            auto in_data = this->inLink->receiveData();
            if (this->outLink->isReady()) {
                this->outLink->sendData(in_data->data.get(), in_data->size);
            }
        }
        // if (this->outLink->isReady() && this->outLink->hasData()) {
        //     auto out_data = this->outLink->receiveData();
        //     if (this->inLink->isReady()) {
        //         this->inLink->sendData(out_data->data.get(), out_data->size);
        //     }
        // }
    }
}


DataBrigde::~DataBrigde() {
    delete this->outLink;
}
