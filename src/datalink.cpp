#include "../include/datalink.h"
#include "../include/tcplink.h"


std::shared_ptr<Datalink> Datalink::TcpServer(int port, float no_data_timeout_ms, int max_incommming_messages_in_queue)
{
    return std::shared_ptr<Datalink>((Datalink *)new TCPLink(port, no_data_timeout_ms, false, max_incommming_messages_in_queue));
}
std::shared_ptr<Datalink> Datalink::TcpClient(const char *host, int port, float no_data_timeout_ms, int max_incommming_messages_in_queue)
{
    return std::shared_ptr<Datalink>((Datalink *)new TCPLink(host, port, no_data_timeout_ms, false, max_incommming_messages_in_queue));
}

long get_exec_time_ms(std::chrono::high_resolution_clock::time_point &start)
{
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    return duration_ms.count();
}

bool Datalink::writeWithAck(uint8_t *payload, long size, double timestamp)
{
    auto exec_start = std::chrono::high_resolution_clock::now();

    long timeout = static_cast<long>(timeout_ms());

    while (!isReady())
    {
        if (timeout > 0 && get_exec_time_ms(exec_start) > timeout)
            return false;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    bool acked = false;
    bool failed_ack_wait = false;
    int wait_loops = 0;

    while (!acked)
    {

        write(payload, size, timestamp);

        wait_loops = 0;
        failed_ack_wait = false;

        while (!failed_ack_wait && (!isReady() || !hasData()))
        {
            if (timeout > 0 && get_exec_time_ms(exec_start) > timeout)
                return false;

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            wait_loops += 1;

            if (wait_loops > 100)
                failed_ack_wait = true;
        }

        if (!failed_ack_wait)
        {
            auto [ack, _] = readMessage();
            acked = ack.size() > 0 && ack[0] == 1;
        }
    }
    return true;
}

#define ACK uint8_t[]{1};
#define NO_ACK uint8_t[]{0};

std::tuple<std::vector<uint8_t>, double> Datalink::readMessageWithAck()
{
    auto exec_start = std::chrono::high_resolution_clock::now();
    long timeout = static_cast<long>(timeout_ms());

    while (!isReady())
    {
        if (timeout > 0 && get_exec_time_ms(exec_start) > timeout)
            return {std::vector<uint8_t>(), -1};

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    bool recvd = false;

    while (!recvd)
    {
        while (!isReady() || !hasData())
        {
            if (timeout > 0 && get_exec_time_ms(exec_start) > timeout)
                return {std::vector<uint8_t>(), -1};

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        auto [raw, timestamp] = readMessage();

        if (raw.size() <= 0)
        {
            uint8_t no_ack[] = {0};
            write(no_ack, sizeof(uint8_t), 1);
        }
        else
        {
            uint8_t ack[] = {1};
            write(ack, sizeof(uint8_t), 1);
            recvd = true;
            return {raw, timestamp};
        }
    }

    return {std::vector<uint8_t>(), -1};
}