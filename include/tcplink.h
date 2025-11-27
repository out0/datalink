#include <memory>
#include <thread>
#include "../include/datalink.h"

#define HEADER_INIT_BYTE 32
#define HEADER_LEN 5*sizeof(char)
#define HEADER_FINISH_BYTE 33
#define HEADER_SIZE 2*HEADER_LEN + sizeof(long)

#define FOOTER_BYTE 34
#define FOOTER_SIZE 5*sizeof(char)

#include <queue>
#include <vector>

class TCPLink {
private:
    char * _host;
    int _port;
    int _state;
    int _listenSockFd;
    int _connSockFd;
    double _timeoutStart;
    double _timeout_ms;
    char _default_header[HEADER_SIZE + 1];
    char _default_footer[FOOTER_SIZE + 1];
    char _read_header[HEADER_SIZE + 1];
    char _read_footer[FOOTER_SIZE + 1];
    bool _link_ready;
    bool _is_running;
    std::unique_ptr<std::thread> _linkRunThread;
    std::queue<std::vector<char>> _incommingMessages;

    void _loop();
    int _openLink();
    int _bindPort();
    int _acceptIncommingConnection();
    int _openConnection();
    int _waitConnectionToServerIsCompleted();
    int _dataTransfer();
    void _linkRun();
    bool _readFromSocket(int sockfd, char *buffer, long size);
    long _readMessageHeader();
    bool _readMessageFooter();
    void _rstTimeout();
    bool _checkTimeout();
    std::vector<char> _read_raw();
    std::mutex _incomming_data_mtx;
    

public:
    TCPLink(const char *server, int port, float no_data_timeout_ms = -1);
    TCPLink(char *server, int port, float no_data_timeout_ms = -1);
    TCPLink(int port, float no_data_timeout_ms = -1);
    ~TCPLink();

    bool isReady();
    bool write(const char *payload, long payload_size);
    bool hasData();
    std::vector<char> readMessage();
    
};