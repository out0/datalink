#include <string>
#include <memory>
#include <cstring>
#include "../include/datalink.h"
#include "../include/tcplink.h"

// #define DEBUG_BIND 1

extern uint8_t *data_encode(int8_t *data, long size);
extern uint8_t *data_encode(int *data, long size);
extern uint8_t *data_encode(long *data, long size);
extern uint8_t *data_encode(float *data, long size);
extern uint8_t *data_encode(double *data, long size);
extern int *data_decode_int(uint8_t *data, long size);
extern int8_t *data_decode_int8(uint8_t *data, long size);
extern long *data_decode_long(uint8_t *data, long size);
extern float *data_decode_float(uint8_t *data, long size);
extern double *data_decode_double(uint8_t *data, long size);

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

        bool write_raw_data(void *link, uint8_t *data, long size, double timestamp)
        {
#ifdef DEBUG_BIND
                printf("writing string to the Datalink %p size: %ld\n", link, size);
#endif
                return ((TCPLink *)link)->write(data, size, timestamp);
        }

        bool is_ready(void *link)
        {
                return ((TCPLink *)link)->isReady();
        }

        long next_message_size(void *link)
        {
                auto ptr = ((TCPLink *)link);
                return ptr->readMessageSize();
        }

        long read_next_message(void *link, uint8_t *buffer, long size, double *timestamp)
        {
                auto ptr = ((TCPLink *)link);
                return ptr->readMessageToBuffer(buffer, size, timestamp);
        }

        bool has_data(void *link)
        {
                auto ptr = ((TCPLink *)link);
                return ptr->hasData();
        }

        void clear_buffer(void *link)
        {
                auto ptr = ((TCPLink *)link);
                return ptr->clearBuffer();
        }

        bool write_keep_alive_message(void *link) {
                auto ptr = ((TCPLink *)link);
                return ptr->writeKeepAlive();                
        }
}