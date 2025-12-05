#include <memory>
#include <thread>
#include "../include/datalink.h"

#define HEADER_INIT_BYTE 32
#define HEADER_LEN 5 * sizeof(uint8_t)
#define HEADER_FINISH_BYTE 33
#define HEADER_SIZE HEADER_LEN + sizeof(long) + sizeof(double) + HEADER_LEN

//#define HEADER_SIZE 26*sizeof(uint8_t)

#define FOOTER_BYTE 34
#define FOOTER_SIZE 5 * sizeof(uint8_t)

#include <queue>
#include <vector>
#include <sys/select.h>

class TCPLink
{
private:
    char *_host;
    int _port;
    int _state;
    int _listenSockFd;
    int _connSockFd;

    double _timeoutStart;
    double _timeout_ms;
    uint8_t _default_header[HEADER_SIZE + 1];
    uint8_t _default_footer[FOOTER_SIZE + 1];
    uint8_t _read_header[HEADER_SIZE + 1];
    uint8_t _read_footer[FOOTER_SIZE + 1];
    bool _link_ready;
    bool _is_running;
    bool _write_with_invalid_state;
    std::unique_ptr<std::thread> _linkRunThread;
    std::queue<std::tuple<std::vector<uint8_t>, double>> _incommingMessages;

    void _loop();
    int _openLink();
    int _bindPort();
    int _acceptIncommingConnection();
    int _openConnection();
    int _waitConnectionToServerIsCompleted();
    int _dataTransfer();
    void _linkRun();
    bool _readFromSocket(int sockfd, uint8_t *buffer, long size);
    std::tuple<long, double> _readMessageHeader();
    bool _readMessageFooter();
    void _rstTimeout();
    bool _checkTimeout();
    std::tuple<std::vector<uint8_t>, double> _read_raw();
    std::mutex _incomming_data_mtx;

public:
    TCPLink(const char *server, int port, float no_data_timeout_ms = -1);
    TCPLink(char *server, int port, float no_data_timeout_ms = -1);
    TCPLink(int port, float no_data_timeout_ms = -1);
    ~TCPLink();

    bool isReady();
    bool write(const uint8_t *payload, long payload_size, double timestamp);
    bool writeKeepAlive();
    bool hasData();

    std::tuple<std::vector<uint8_t>, double> readMessage();
    long readMessageSize();
    long readMessageToBuffer(uint8_t *buffer, long size, double *timestamp);
    void clearBuffer();
};