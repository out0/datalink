#include <sys/socket.h>
#include <stdio.h>
#include <chrono>
#include <vector>
#include <iostream>
#include <sys/socket.h> // Include for socket APIs
#include <netinet/in.h> // Include for internet protocols
#include <cstring>      // Include for string operations
#include <unistd.h>     // Include for POSIX operating system API
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include "../include/tcplink.h"

#define STATE_CONNECTION_CLOSED 0
#define STATE_WAIT_LISTEN 1
#define STATE_CONNECTION_OPENED 2
#define STATE_WAIT_CONNECTION_COMPLETION 3
#define STATE_CONNECTION_CLOSING 4
#define NON_BLOCKING 1

#define DATALINK_MTU 1024
#define DATALINK_MRU 1024
#define DEBUG_DATA 1

extern timeval set_timeout_ms(double timeout_ms);

inline long min(long a, long b)
{
    return a < b ? a : b;
}
inline double time_now()
{
    return std::chrono::duration_cast<std::chrono::duration<double>>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}
inline void wait_ms(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

bool write_header(int sockfd, char *default_header, long payload_size)
{
    longp p;
    p.val = payload_size;

    for (int i = 0; i < sizeof(long); i++)
    {
        default_header[i + HEADER_LEN] = p.bval[i];
    }

    // printf ("sent header: ");
    // for (int i = 0; i < HEADER_SIZE; i++)
    // {
    //     printf (" %d", default_header[i]);
    // }
    // printf ("\n");

    int i = send(sockfd, default_header, HEADER_SIZE, MSG_NOSIGNAL);

    return i == HEADER_SIZE;
}
bool write_footer(int sockfd, char *default_footer)
{
    // printf ("sent footer: ");
    // for (int i = 0; i < FOOTER_SIZE; i++)
    // {
    //     printf (" %d", default_footer[i]);
    // }
    // printf ("\n");

    return send(sockfd, default_footer, FOOTER_SIZE, MSG_NOSIGNAL) == FOOTER_SIZE;
}
bool check_connection_lost(int socketResult, int errorCode)
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
bool check_repeat_byte(char *buffer, int startPos, int count, int byte)
{
    for (int i = startPos; i < startPos + count; i++)
        if (buffer[i] != byte)
            return false;
    return true;
}

bool TCPLink::_readFromSocket(int sockfd, char *buffer, long size)
{
    if (!_link_ready)
        return false;

    long read_size = 0;
    long max_package_size = -1;

    while (read_size < size)
    {
        max_package_size = min(size - read_size, DATALINK_MRU);

        int partialSize = read(sockfd, buffer + read_size, max_package_size);

        if (check_connection_lost(partialSize, errno))
            return false;

        if (_checkTimeout())
            return false;

        if (partialSize < 0)
        {
            continue;
            // #ifdef DEBUG_DATA
            //             printf("read from socket returned -1 data for partial read\n");
            // #endif
            //             return false;
        }

        if (!_is_running)
            return false;

        read_size += partialSize;

        if (partialSize == 0 && read_size < size)
            return false;

        _rstTimeout();
    }

    return true;
}
long TCPLink::_readMessageHeader()
{
    if (!_readFromSocket(_connSockFd, _read_header, HEADER_SIZE))
    {
        return -1;
    }

    // printf("recv header: ");
    // for (int i = 0; i < HEADER_SIZE; i++) {
    //     printf (" %d", _read_header[i]);
    // }
    // printf("\n");

    if (!check_repeat_byte(_read_header, 0, HEADER_LEN, HEADER_INIT_BYTE))
    {
        // printf ("invalid header start\n");
        return -1;
    }

    longp res;
    for (int i = HEADER_LEN; i < HEADER_LEN + sizeof(long); i++)
        res.bval[i - HEADER_LEN] = _read_header[i];

    if (!check_repeat_byte(_read_header, HEADER_LEN + sizeof(long), HEADER_LEN, HEADER_FINISH_BYTE))
    {
        // printf ("invalid header end\n");
        return -1;
    }

    // printf ("valid header with %ld bytes\n", res.val);

    return res.val;
}
bool TCPLink::_readMessageFooter()
{
    if (!_readFromSocket(_connSockFd, _read_footer, FOOTER_SIZE))
    {
        // printf ("[FOOTER] error reading from socket\n");
        return false;
    }

    // bool v = check_repeat_byte(_read_footer, 0, FOOTER_SIZE, FOOTER_BYTE);

    // if (v) return true;
    // printf ("[FOOTER] error: data mismatch: ");
    // for (int i = 0; i < FOOTER_SIZE; i++) {
    //     printf (" %d", _read_footer[i]);
    // }
    // printf ("\n");
    // return false;
    return check_repeat_byte(_read_footer, 0, FOOTER_SIZE, FOOTER_BYTE);
    ;
}

void TCPLink::_rstTimeout()
{
    if (_timeout_ms <= 0)
        return;
    _timeoutStart = time_now();
}
bool TCPLink::_checkTimeout()
{
    if (_timeout_ms <= 0)
        return false;

    if (_timeoutStart < 0)
        _timeoutStart = time_now();

    if (1000 * (time_now() - _timeoutStart) > _timeout_ms)
    {
#ifdef DEBUG
        printf("[datalink] TIMEOUT\n");
#endif
        return true;
    }
    return false;
}

bool TCPLink::write(const char *payload, long payload_size)
{
    if (!_link_ready)
        return false;

    long transmited_size = 0;

    if (!write_header(_connSockFd, _default_header, payload_size))
    {
        fprintf(stderr, "!! [datalink error] unable to send message header\n");
        return false;
    }

    while (transmited_size < payload_size)
    {
        long max_package_size = min(payload_size - transmited_size, DATALINK_MTU);
        long partialSize = send(_connSockFd, payload + transmited_size, max_package_size, MSG_NOSIGNAL);

        if (check_connection_lost(partialSize, errno))
            return false;

        if (_checkTimeout())
            return false;

        transmited_size += partialSize;
    }

    if (!write_footer(_connSockFd, _default_footer))
    {
        fprintf(stderr, "!! [datalink error] unable to send message footer\n");
        return false;
    }

    return true;
}
std::vector<char> TCPLink::_read_raw()
{
    if (!_link_ready)
        return {};

    long size = _readMessageHeader();
    if (size <= 0)
        return {};

    std::vector<char> res;
    res.resize(size);

    if (_readFromSocket(_connSockFd, &res[0], size) && _readMessageFooter())
    {
        return res;
    }

    res.clear();
    return std::vector<char>();
}

void build_default_header(char *header)
{
    memset(header, 0, HEADER_SIZE);

    for (int i = 0; i < HEADER_LEN; i++)
        header[i] = HEADER_INIT_BYTE;

    int st = HEADER_LEN + sizeof(long);

    for (int i = 0; i < HEADER_LEN; i++)
        header[i + st] = HEADER_FINISH_BYTE;
}
char *build_default_footer(char *footer)
{
    memset(footer, 0, FOOTER_SIZE);

    for (int i = 0; i < FOOTER_SIZE; i++)
        footer[i] = FOOTER_BYTE;
    return footer;
}

void TCPLink::_loop()
{
    switch (_state)
    {
    case STATE_CONNECTION_CLOSED:
        _state = _openLink();
        break;
    case STATE_WAIT_LISTEN:
        _state = _acceptIncommingConnection();
        break;
    case STATE_CONNECTION_OPENED:
        _state = _dataTransfer();
        break;
    case STATE_WAIT_CONNECTION_COMPLETION:
        _state = _waitConnectionToServerIsCompleted();
        break;
    case STATE_CONNECTION_CLOSING:
        close(_connSockFd);
        _state = STATE_CONNECTION_CLOSED;
        break;
    default:
        _state = STATE_CONNECTION_CLOSED;
        break;
    }
}

int TCPLink::_openLink()
{
    if (_host == nullptr)
        return _bindPort();

    return _openConnection();
}

int TCPLink::_bindPort()
{
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    _listenSockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_listenSockFd < 0)
    {
        perror("[datalink] socket");
        return STATE_CONNECTION_CLOSING;
    }

    if (setsockopt(_listenSockFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("[datalink] setsockopt"); // Print error if setting socket options fails
        return STATE_CONNECTION_CLOSING;
    }

    address.sin_family = AF_INET;         // Set address family to AF_INET (IPv4)
    address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP
    address.sin_port = htons(_port);      // Set port

#ifdef NON_BLOCKING
    int flags = fcntl(_listenSockFd, F_GETFL, 0);
    if (flags == -1)
    {
        // Error handling for fcntl(F_GETFL)
    }
    if (fcntl(_listenSockFd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        // Error handling for fcntl(F_SETFL)
    }
#endif

    if (bind(_listenSockFd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("[datalink] bind failed");
        return STATE_CONNECTION_CLOSING;
    }

    return STATE_WAIT_LISTEN;
}

int TCPLink::_acceptIncommingConnection()
{
#ifdef DEBUG
    printf("Accept incoming: init\n");
#endif
    _link_ready = false;

    if (listen(_listenSockFd, 10) < 0)
        return false;

    // timeval time = set_timeout_ms(_timeout_ms);

    // fd_set master;
    // FD_ZERO(&master);
    // FD_SET(_listenSockFd, &master);

    // if (select(0, &master, NULL, NULL, &time) == -1)
    // {
    //     close(_listenSockFd);
    //     return STATE_CONNECTION_CLOSING;
    // }

    // if (!FD_ISSET(_listenSockFd, &master))
    // {
    //     close(_listenSockFd);
    //     return STATE_CONNECTION_CLOSING;
    // }

    struct sockaddr_in client_address;
    socklen_t cli_addr_size = sizeof(client_address);
    std::memset(&client_address, 0, cli_addr_size);

    bool connected = false;
     _link_ready = false;
    while (_is_running && !connected)
    {
        _connSockFd = accept(_listenSockFd, (struct sockaddr *)&client_address, (socklen_t *)&cli_addr_size);

        if (_connSockFd >= 0)
            connected = true;
    }

#ifdef DEBUG
    printf("Accept incoming: client connected\n");
#endif

    if (!_is_running)
    {
        return STATE_CONNECTION_CLOSING;
    }

    _link_ready = true;
    return STATE_CONNECTION_OPENED;

    // FD_SET(connSock, &master); // add to the file descriptor
    // close(sock);
}

int asyncTCPConnectionSucceed(int sockfd)
{
    int error = 0;
    socklen_t len = sizeof(error);
    int getsockopt_status = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);

    if (getsockopt_status == -1)
    {
        // Handle getsockopt error
        return -1;
    }
    else
    {
        if (error == 0)
        {
            // Connection successful
            return 0;
        }
        else
        {
            // Connection failed with 'error' code
            return -3;
        }
    }
}

int TCPLink::_openConnection()
{
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

#ifdef DEBUG
    printf("open connection: init\n");
#endif

    _connSockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_connSockFd < 0)
    {
        perror("[datalink] socket");
        return STATE_CONNECTION_CLOSING;
    }

    struct hostent *server_info = gethostbyname(_host);
    if (server_info == nullptr)
    {
        close(_connSockFd);
        printf("[datalink] resolve hostname returned empty information for host %s\n", _host);
        perror("[datalink] reason");
        return STATE_CONNECTION_CLOSING;
    }

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(_port);
    std::memcpy(&server_addr.sin_addr, server_info->h_addr, server_info->h_length);

#ifdef NON_BLOCKING
    // perform a non-blocking connect and wait up to _timeout_ms for it to complete
    int flags = fcntl(_connSockFd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("[datalink] fcntl F_GETFL");
        close(_connSockFd);
        return STATE_CONNECTION_CLOSING;
    }
    if (fcntl(_connSockFd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("[datalink] fcntl F_SETFL O_NONBLOCK");
        close(_connSockFd);
        return STATE_CONNECTION_CLOSING;
    }
#endif
    // Connect to the server
    bool connected = false;
    while (_is_running && !connected)
    {
        int ret = connect(_connSockFd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (ret == -1)
        {
            if (errno != EINPROGRESS)
            {
                perror("[datalink] connect");
                return STATE_CONNECTION_CLOSING;
            }

            return STATE_WAIT_CONNECTION_COMPLETION;
        }
        else
        {
            connected = true;
        }
        // close(_connSockFd);

#ifdef DEBUG
        printf("[datalink] failed to connect to %s, %d\n", host, port);
        perror("[datalink] connect");
#endif
    }

#ifdef DEBUG
    printf("[datalink] connected\n");
#endif

    struct timeval time = set_timeout_ms(_timeout_ms);
    setsockopt(_connSockFd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&time, sizeof(time));

    _link_ready = true;
#ifdef DEBUG
    printf("open connection: connected\n");
#endif
    return STATE_CONNECTION_OPENED;
}

int TCPLink::_waitConnectionToServerIsCompleted()
{
    int err = asyncTCPConnectionSucceed(_connSockFd);
    if (err == -1)
        return STATE_WAIT_CONNECTION_COMPLETION;
    if (err == -3)
        return STATE_CONNECTION_CLOSING;

    struct timeval time = set_timeout_ms(_timeout_ms);
    setsockopt(_connSockFd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&time, sizeof(time));
    _rstTimeout();
    _link_ready = true;

#ifdef DEBUG
    printf("open connection: connected\n");
#endif
    return STATE_CONNECTION_OPENED;
}

void TCPLink::_linkRun()
{
    while (_is_running)
        _loop();
}

TCPLink::TCPLink(const char *server, int port, float no_data_timeout_ms)
{
    if (server)
    {
        size_t len = std::strlen(server) + 1;
        _host = new char[len];
        std::memcpy(_host, server, len);
    }
    else
    {
        _host = nullptr;
    }
    _port = port;
    _state = STATE_CONNECTION_CLOSED;
    _connSockFd = 0;
    _timeoutStart = -1;
    _timeout_ms = static_cast<double>(no_data_timeout_ms);
    build_default_header(_default_header);
    build_default_footer(_default_footer);
    _link_ready = false;
    _is_running = true;
    _linkRunThread = std::make_unique<std::thread>(&TCPLink::_linkRun, this);
}

TCPLink::TCPLink(char *server, int port, float no_data_timeout_ms)
{
    if (server)
    {
        size_t len = std::strlen(server) + 1;
        _host = new char[len];
        std::memcpy(_host, server, len);
    }
    else
    {
        _host = nullptr;
    }
    _port = port;
    _state = STATE_CONNECTION_CLOSED;
    _connSockFd = 0;
    _timeoutStart = -1;
    _timeout_ms = static_cast<double>(no_data_timeout_ms);
    build_default_header(_default_header);
    build_default_footer(_default_footer);
    _link_ready = false;
    _is_running = true;
    _linkRunThread = std::make_unique<std::thread>(&TCPLink::_linkRun, this);
}
TCPLink::TCPLink(int port, float no_data_timeout_ms)
{
    _host = nullptr;
    _port = port;
    _state = STATE_CONNECTION_CLOSED;
    _connSockFd = 0;
    _timeoutStart = -1;
    _timeout_ms = static_cast<double>(no_data_timeout_ms);
    build_default_header(_default_header);
    build_default_footer(_default_footer);
    _link_ready = false;
    _is_running = true;
    _linkRunThread = std::make_unique<std::thread>(&TCPLink::_linkRun, this);
}
TCPLink::~TCPLink()
{
    _is_running = false;

    if (_linkRunThread)
    {
        if (_linkRunThread->joinable())
        {
            _linkRunThread->join();
        }
        _linkRunThread.reset();
    }

    if (_connSockFd > 0)
        close(_connSockFd);
    if (_listenSockFd > 0)
        close(_listenSockFd);

    delete[] _host;
}

bool TCPLink::isReady()
{
    return _link_ready;
}

bool TCPLink::hasData()
{
    return _incommingMessages.size() > 0;
}
std::vector<char> TCPLink::readMessage()
{
    std::lock_guard<std::mutex> guard(_incomming_data_mtx);
    auto data = _incommingMessages.front();
    _incommingMessages.pop();
    return data;
}
int TCPLink::_dataTransfer()
{
    auto raw = _read_raw();

    if (_checkTimeout())
    {
        if (_host == nullptr)
            return STATE_WAIT_LISTEN;
        return STATE_CONNECTION_CLOSING;
    }

    if (raw.size() > 0)
    {
        // printf("recv valid data, acquiring buffer\n");
        std::lock_guard<std::mutex> guard(_incomming_data_mtx);
        // printf("writing the message to the queue\n");
        _incommingMessages.push(raw);
        _rstTimeout();
    }

    return STATE_CONNECTION_OPENED;
}
