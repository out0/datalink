#include "../include/tcplink.h"
#include "../include/databridge.h"

DataBrigde::DataBrigde(Datalink *left, Datalink *right)
{
    _left = left;
    _right = right;
    _run = true;
    left->setForwardMode();
    right->setForwardMode();
    _leftToRight = std::make_unique<std::thread>(&DataBrigde::_leftToRightRun, this);
    _rightToLeft = std::make_unique<std::thread>(&DataBrigde::_rightToLeftRun, this);
}

DataBrigde::~DataBrigde()
{
    _run = false;
    if (_leftToRight)
    {
        if (_leftToRight->joinable())
        {
            _leftToRight->join();
        }
        _leftToRight.reset();
    }
    if (_rightToLeft)
    {
        if (_rightToLeft->joinable())
        {
            _rightToLeft->join();
        }
        _rightToLeft.reset();
    }
}

void DataBrigde::_leftToRightRun()
{
    while (_run)
    {
        if (_left->isReady())
        {
            if (_left->hasData())
            {
                auto [msg, timestamp] = _left->readMessage();
                if (_right->isReady())
                    _right->write(&msg[0], msg.size(), timestamp);
                continue;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
void DataBrigde::_rightToLeftRun()
{
    uint8_t *pl = new u_int8_t[1];
    pl[0] = 'a';

    while (_run)
    {
        if (_right->isReady())
        {
            if (_right->hasData())
            {
                auto [msg, timestamp] = _right->readMessage();
                if (_left->isReady())
                    _left->write(&msg[0], msg.size(), timestamp);
                continue;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}