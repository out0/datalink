#include <string>
#include <memory>
#include <cstring>
#include "../include/datalink.h"
#include "../include/tcplink.h"

// #define DEBUG_BIND 1

extern char *data_encode(int8_t *data, long size);
extern char *data_encode(int *data, long size);
extern char *data_encode(long *data, long size);
extern char *data_encode(float *data, long size);
extern char *data_encode(double *data, long size);
extern int *data_decode_int(char *data, long size);
extern int8_t *data_decode_int8(char *data, long size);
extern long *data_decode_long(char *data, long size);
extern float *data_decode_float(char *data, long size);
extern double *data_decode_double(char *data, long size);

extern "C"
{
        void *init_tcp_server(int port, float timeout)
        {
                auto link = new TCPLink(port, timeout);
#ifdef DEBUG_BIND
                printf("creating a Datalink server %p with port: %d\n", link, port);
#endif
                return link;
        }

        void *init_tcp_client(char *host, int port, float timeout)
        {
                auto link = new TCPLink(host, port, timeout);
#ifdef DEBUG_BIND
                printf("creating a Datalink client %p to host %s and port: %d\n", link, host, port);
#endif
                return link;
        }

        void destroy_tcp_link(void *link)
        {
#ifdef DEBUG_BIND
                printf("destroying the Datalink %p\n", link);
#endif
                delete ((TCPLink *)link);
        }

        bool write_str_link(void *link, char *data, long size)
        {
#ifdef DEBUG_BIND
                printf("writing string to the Datalink %p size: %ld\n", link, size);
#endif
                return ((TCPLink *)link)->write(data, size);
        }
        bool write_int_array(void *link, int32_t *data, long size)
        {
#ifdef DEBUG_BIND
                printf("writing string to the Datalink %p size: %ld\n", link, size);
#endif
                char *p = data_encode(data, size);
                bool res = ((TCPLink *)link)->write(p, size * sizeof(int));
                delete[] p;
                return res;
        }

        bool write_long_array(void *link, long *data, long size)
        {
#ifdef DEBUG_BIND
                printf("writing string to the Datalink %p size: %ld\n", link, size);
#endif
                char *p = data_encode(data, size);
                int res = ((TCPLink *)link)->write(p, size * sizeof(long));
                delete[] p;
                return res;
        }
        bool write_float_array(void *link, float *data, long size)
        {
#ifdef DEBUG_BIND
                printf("writing string to the Datalink %p size: %ld\n", link, size);
#endif
                char *p = data_encode(data, size);
                int res = ((TCPLink *)link)->write(p, size * sizeof(float));
                delete[] p;
                return res;
        }
        bool write_double_array(void *link, double *data, long size)
        {
#ifdef DEBUG_BIND
                printf("writing string to the Datalink %p size: %ld\n", link, size);
#endif
                char *p = data_encode(data, size);
                int res = ((TCPLink *)link)->write(p, size * sizeof(double));
                delete[] p;
                return res;
        }
        bool write_int8_array(void *link, int8_t *data, long size)
        {
#ifdef DEBUG_BIND
                printf("writing string to the Datalink %p size: %ld\n", link, size);
#endif
                char *p = data_encode(data, size);
                int res = ((TCPLink *)link)->write(p, size * sizeof(int8_t));
                delete[] p;
                return res;
        }

        bool is_ready(void *link)
        {
                return ((TCPLink *)link)->isReady();
        }

        char *next_message(void *link, long *size)
        {
                auto ptr = ((TCPLink *)link);
                return ptr->readMessage(size);
        }
        int *next_message_int_array(void *link, long *size)
        {
                auto ptr = ((TCPLink *)link);
                auto data = ptr->readMessage();
                // printf ("[C++] INT package received data size: %ld\n", data.size());
                *size = data.size() / sizeof(int32_t);
                return data_decode_int(&data[0], *size);
        }
        int8_t *next_message_int8_array(void *link, long *size)
        {
                auto ptr = ((TCPLink *)link);
                auto data = ptr->readMessage();
                // printf ("[C++] INT package received data size: %ld\n", data.size());
                *size = data.size() / sizeof(int32_t);
                return data_decode_int8(&data[0], *size);
        }
        long *next_message_long_array(void *link, long *size)
        {
                auto ptr = ((TCPLink *)link);
                auto data = ptr->readMessage();
                // printf ("[C++] LONG package received data size: %ld\n", data.size());
                *size = data.size() / sizeof(long);
                return data_decode_long(&data[0], *size);
        }
        float *next_message_float_array(void *link, long *size)
        {
                auto ptr = ((TCPLink *)link);
                auto data = ptr->readMessage();
                // printf ("[C++] FLOAT package received data size: %ld\n", data.size());
                *size = data.size() / sizeof(float);
                return data_decode_float(&data[0], *size);
        }
        double *next_message_double_array(void *link, long *size)
        {
                auto ptr = ((TCPLink *)link);
                auto data = ptr->readMessage();
                // printf ("[C++] DOUBLE package received data size: %ld\n", data.size());
                *size = data.size() / sizeof(double);
                return data_decode_double(&data[0], *size);
        }

        void free_memory(char *msg)
        {
                delete[] msg;
        }
        void free_memory_int(int *msg)
        {
                delete[] msg;
        }

        void free_memory_int8(int8_t *msg)
        {
                delete[] msg;
        }
        
        void free_memory_long(long *msg)
        {
                delete[] msg;
        }
        void free_memory_float(float *msg)
        {
                delete[] msg;
        }
        void free_memory_double(double *msg)
        {
                delete[] msg;
        }
        bool has_data(void *link)
        {
                auto ptr = ((TCPLink *)link);
                return ptr->hasData();
        }
}