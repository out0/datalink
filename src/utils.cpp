#include <chrono>

timeval set_timeout_ms(double timeout_ms)
{
    timeval time;
    long s = static_cast<long>(timeout_ms / 1000);
    long us = static_cast<long>((timeout_ms - 1000 * s) * 1000);
    time.tv_sec = s;
    time.tv_usec = us;
    // printf ("set_timeout_ms: s = %ld, us=%ld\n", s, us);
    return time;
}
