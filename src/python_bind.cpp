#include <string>
#include <memory>
#include <cstring>
#include "../include/datalink.h"
#include "../include/tcplink.h"

// #define DEBUG_BIND 1

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

        bool is_ready(void *link)
        {
                return ((TCPLink *)link)->isReady();
        }

        char *next_message(void *link, long *size)
        {
                auto ptr = ((TCPLink *)link);
                return ptr->readMessage(size);
        }
        void free_memory(char *msg)
        {
                delete[] msg;
        }

        bool has_data(void *link)
        {
                auto ptr = ((TCPLink *)link);
                return ptr->hasData();
        }
}