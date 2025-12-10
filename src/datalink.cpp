#include "../include/datalink.h"
#include "../include/tcplink.h"

std::shared_ptr<Datalink> Datalink::TcpServer(int port, float no_data_timeout_ms)
{
    return std::shared_ptr<Datalink>((Datalink *)new TCPLink(port, no_data_timeout_ms, false));
}
std::shared_ptr<Datalink> Datalink::TcpClient(const char *host, int port, float no_data_timeout_ms)
{
    return std::shared_ptr<Datalink>((Datalink *)new TCPLink(host, port, no_data_timeout_ms, false));
}
