#ifndef __DATA_BRIGDGE_H
#define __DATA_BRIGDGE_H

#include "datalink.h"
#include <thread>
#include <memory>

class DataBrigde
{
    Datalink *_left;
    Datalink *_right;
    std::unique_ptr<std::thread> _leftToRight;
    std::unique_ptr<std::thread> _rightToLeft;
    bool _run;
    void _leftToRightRun();
    void _rightToLeftRun();

public:
    DataBrigde(Datalink *left, Datalink *right);
    ~DataBrigde();
};

#endif