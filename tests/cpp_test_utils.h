#ifndef __CPP_TEST_UTILS_H
#define __CPP_TEST_UTILS_H

#include <stdio.h>
#include <thread>
#include <memory>
#include <functional>
#include <chrono>
#include <iostream>


typedef std::unique_ptr<std::thread> thr;

static double time_now()
{
    return std::chrono::duration_cast<std::chrono::duration<double>>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

static double start = -1;

static bool check_timeout(double timeout_s)
{
    if (start <= 0)
    {
        start = time_now();
        return false;
    }
    return time_now() - start > timeout_s;
}

static thr parallelExecute(std::function<void()> method)
{
    return thr(new std::thread(method));
}

static void parallelWaitFinish(thr &t)
{
    t->join();
}

static bool executeUntilSuccessOrTimeout(std::function<bool()> method, int timeout_ms)
{
    long start = time_now();
    while (!method())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (time_now() - start > timeout_ms)
        {
            printf("[test] timeout\n");
            return false;
        }
    }

    return true;
}

static void wait_ms(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}


#endif
