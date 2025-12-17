#ifndef __SERIALIZER_CPP_H
#define __SERIALIZER_CPP_H

#include <string>
#include <memory>
#include <iostream>
#include <string.h>
#include <vector>

class Serializer
{
    std::vector<uint8_t> buffer;
    long bufferWritePos;
    long bufferReadPos;
    long bufferSize;
    const long header_size = sizeof(long);

    union
    {
        long val;
        uint8_t valb[sizeof(long)];
    } longp;

    union
    {
        int val;
        uint8_t valb[sizeof(int)];
    } intp;

    void set_buffer(long size);

    void _write(void *src, long size, bool header);

public:
    Serializer();

    Serializer(uint8_t *initial_buffer, long size);

    Serializer(std::vector<uint8_t> raw);

    Serializer(long initial_size);

    ~Serializer();

    void write(void *buff, long size);
    void read(void *dest, long size);

    void write(std::string s);
    std::string read_string();

    template <typename T>
    void write(T val)
    {
        union
        {
            T val;
            uint8_t valb[sizeof(T)];
        } converter;

        converter.val = val;
        _write(converter.valb, sizeof(T), false);
    }

    template <typename T>
    T read()
    {
        union
        {
            T val;
            uint8_t valb[sizeof(T)];
        } converter;

        read(converter.valb, sizeof(T));
        return converter.val;
    }

    std::vector<uint8_t> get();

    template <typename T>
    T *read_vector()
    {
        if (bufferReadPos + header_size > bufferSize)
        {
            return nullptr;
        }
        uint8_t *ptr = &buffer[0];
        memcpy(longp.valb, ptr + bufferReadPos, header_size);
        long size = longp.val;

        if (bufferReadPos + header_size + size > bufferSize)
        {
            return nullptr;
        }

        T *ret = new T[size];

        memcpy(ret, ptr + bufferReadPos + header_size, size);
        bufferReadPos = bufferReadPos + header_size + size;
        return ret;
    }

    template <typename T>
    bool read_ptr(T *out)
    {
        if (bufferReadPos + header_size > bufferSize)
        {
            return false;
        }
        uint8_t *ptr = &buffer[0];
        memcpy(longp.valb, ptr + bufferReadPos, header_size);
        long size = longp.val;

        if (bufferReadPos + header_size + size > bufferSize)
        {
            return false;
        }

        memcpy(out, ptr + bufferReadPos + header_size, size);
        bufferReadPos = bufferReadPos + header_size + size;
        return true;
    }
};

#endif