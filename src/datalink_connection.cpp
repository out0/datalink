#include <iostream>
#include <sys/socket.h> // Include for socket APIs
#include <netinet/in.h> // Include for internet protocols
#include <cstring>      // Include for string operations
#include <unistd.h>     // Include for POSIX operating system API
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include "../include/datalink.h"

#define DATALINK_MRU 1024
#define DATALINK_MTU 1024

//#define DEBUG 1



bool DatalinkConnection::checkConnectionLost(int socketResult, int errorCode)
{
    if (socketResult >= 0)
        return false;

    switch (errorCode)
    {
    case 9:
    case 32:  // broken pipe
    case 50:  // network is down
    case 51:  // network unreachable
    case 52:  // network dropped connection
    case 53:  // aborted
    case 54:  // connection reset by peer
    case 104: // connection reset by peer
#ifdef DEBUG
        printf("[datalink] connection lost code: %d\n", errorCode);
        perror("reason\n");
#endif
        return true;
    case 11:
// #ifdef DEBUG
//         printf("[datalink] connection lost code: %d\n", errorCode);
//         perror("reason\n");
// #endif
        return false;
    default:
        printf("[datalink] unknown error code: %d\n", errorCode);
        perror("reason\n");
        return true;
    }
}

DatalinkConnection::DatalinkConnection(float no_data_timeout_s)
{
    this->noDataTimeout_s = no_data_timeout_s;
    this->timeoutStart = -1;
}
DatalinkConnection::~DatalinkConnection()
{
    if (!this->isOpened)
        closeConnection();
}

bool DatalinkConnection::bindConnection(int port)
{
    this->isOpened = false;

    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("[datalink] socket");
        return false;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("[datalink] setsockopt"); // Print error if setting socket options fails
        return false;
    }

    address.sin_family = AF_INET;         // Set address family to AF_INET (IPv4)
    address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP
    address.sin_port = htons(port);       // Set port to 8080 with proper byte order

    if (bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("[datalink] bind failed");
        return false;
    }

    if (listen(sock, 1) < 0)
        return false;

    return true;
}
bool DatalinkConnection::openConnection(const char *host, int port)
{
    this->isOpened = false;

    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    this->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sock < 0)
    {
        perror("[datalink] socket");
        return false;
    }

    struct hostent *server_info = gethostbyname(host);
    if (server_info == nullptr)
    {
        close(this->sock);
        printf("[datalink] resolve hostname returned empty information for host %s\n", host);
        perror("[datalink] reason");
        return false;
    }

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    std::memcpy(&server_addr.sin_addr, server_info->h_addr, server_info->h_length);

    // Connect to the server
    if (connect(this->sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        close(this->sock);
#ifdef DEBUG        
        printf("[datalink] failed to connect to %s, %d\n", host, port);
        perror("[datalink] connect");
#endif        
        return false;
    }

#ifdef DEBUG
    printf("[datalink] connected\n");
#endif

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(this->sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));

    this->isOpened = true;
    return true;
}
bool DatalinkConnection::socketListen()
{
    this->isOpened = false;

#ifdef DEBUG
    printf("[datalink] listening for incomming connections...\n");
#endif

    timeval time;
    time.tv_sec = 0;
    time.tv_usec = 5000; // 1MS

    fd_set master;
    FD_ZERO(&master);
    FD_SET(sock, &master);

    if (select(0, &master, NULL, NULL, &time) == -1)
        return false;

    if (!FD_ISSET(sock, &master)) // check if we have clients waiting to connect
        return false;

    struct sockaddr_in client_address;
    socklen_t cli_addr_size = sizeof(client_address);
    std::memset(&client_address, 0, cli_addr_size);

    int connSock = accept(sock, (struct sockaddr *)&client_address, (socklen_t *)&cli_addr_size);
    if (connSock < 0)
        return false;

    FD_SET(connSock, &master); // add to the file descriptor

    close(sock);

#ifdef DEBUG
    char ip4[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_address.sin_addr), ip4, INET_ADDRSTRLEN);
    printf("[datalink] connection established to %s\n", ip4);
#endif

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(connSock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));

    this->sock = connSock;
    this->isOpened = true;
    return true;
}
void DatalinkConnection::closeConnection()
{
    this->isOpened = false;
    close(this->sock);
}

bool DatalinkConnection::readFromSocket(char *buffer, long expectedSize)
{
    if (!this->isOpened)
        return false;

    long totalSize = 0;
    long maxBytesToRead = -1;

    while (totalSize < expectedSize)
    {
        maxBytesToRead = DatalinkCommon::min(expectedSize - totalSize, DATALINK_MRU);

        long partialSize = read(this->sock, buffer + totalSize, maxBytesToRead);

        if (checkConnectionLost(partialSize, errno))
            return false;

        if (checkTimeout())
            return false;

        if (partialSize < 0)
            return false;

        totalSize += partialSize;

        if (partialSize == 0 && totalSize < expectedSize)
            return false;

        rstTimeout();
    }

    return true;
}
bool DatalinkConnection::writeToSocket(char *buffer, long expectedSize)
{
    if (!this->isOpened)
        return false;

    long totalSize = 0;
    long maxBytesToWrite = -1;

    while (totalSize < expectedSize)
    {
        maxBytesToWrite = DatalinkCommon::min(expectedSize - totalSize, DATALINK_MTU);

        long partialSize = send(this->sock, buffer + totalSize, maxBytesToWrite, MSG_NOSIGNAL);

        if (checkConnectionLost(partialSize, errno))
            return false;

        if (checkTimeout())
            return false;

        totalSize += partialSize;
        rstTimeout();
    }

    return true;
}

void DatalinkConnection::rstTimeout()
{
    timeoutStart = -1;
}
bool DatalinkConnection::checkTimeout()
{
    if (this->noDataTimeout_s <= 0)
        return false;

    if (timeoutStart <= 0)
    {
        timeoutStart = DatalinkCommon::timeNow();
        return false;
    }
    if (DatalinkCommon::timeNow() - timeoutStart > noDataTimeout_s)
    {
#ifdef DEBUG
        printf("[datalink] TIMEOUT\n");
#endif
        return true;
    }
    return false;
}

bool DatalinkConnection::isConnected() {
    return this->isOpened;
}

