#ifndef __DATA_BRIGDGE_H
#define __DATA_BRIGDGE_H

#include "datalink.h"
#include <thread>
#include <memory>

class DataBrigde
{
    Datalink *inLink;
    Datalink *outLink;
    std::unique_ptr<std::thread> linkRunThread;
    bool run;
    void linkRun();

public:
    DataBrigde(const char *inHost, int inPort, const char *outHost, int outPort, float timeout);
    DataBrigde(int inPort, const char *outHost, int outPort, float timeout);
    DataBrigde(const char *inHost, int inPort, int outPort, float timeout);
    DataBrigde(int inPort, int outPort, float timeout);
    ~DataBrigde();    
};

#endif