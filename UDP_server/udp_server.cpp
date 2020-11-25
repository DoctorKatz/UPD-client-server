//********************************************************************************************************************//
//UDP_SERVER**********************************************************************************************************//
//Created by Nicolay Kats on 15.11.2020*******************************************************************************//
//********************************************************************************************************************//

#include "udp_server.h"
#include "iostream"

#define MAX_DATA_SIZE 1147

using namespace udp_server;

udp_server::UdpServer::UdpServer(int port) : f_port(port)
{
    std::string addr = "0.0.0.0";

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family    = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(f_port);

    if ((f_socket = socket(server_addr.sin_family, SOCK_DGRAM, 0)) < 0)
    {
        throw udp_client_server_runtime_error(("could not create UDP socket for: \"0.0.0.0 \":" + std::to_string(f_port) + "\"").c_str());
    }

    if (bind(f_socket, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        close(f_socket);
        throw udp_client_server_runtime_error(("could not bind UDP socket with: \"0.0.0.0 \":" + std::to_string(f_port) + "\"").c_str());
    }
}

udp_server::UdpServer::~UdpServer()
{
    close(f_socket);
}

int udp_server::UdpServer::get_socket() const
{
    return f_socket;
}

int udp_server::UdpServer::get_port() const
{
    return f_port;
}

int udp_server::UdpServer::recive(char *msg, size_t max_size)
{
    socklen_t len = sizeof(client_addr);
    return ::recvfrom(f_socket, (char *)msg, max_size, MSG_WAITALL, ( struct sockaddr *) &client_addr, &len);
}

int udp_server::UdpServer::send(const char *msg, size_t size)
{
    socklen_t len = sizeof(client_addr);
    return ::sendto(f_socket, (const char *)msg, size, 0, (const struct sockaddr *) &client_addr, len);;
}

int udp_server::UdpServer::timed_recv(char *msg, size_t max_size, int max_wait_ms)
{
    fd_set s;
    FD_ZERO(&s);
    FD_SET(f_socket, &s);
    struct timeval timeout;
    timeout.tv_sec = max_wait_ms / 1000;
    timeout.tv_usec = (max_wait_ms % 1000) * 1000;
    int retval = select(f_socket + 1, &s, &s, &s, &timeout);
    if(retval == -1)
    {
        // select() set errno accordingly
        return -1;
    }
    if(retval > 0)
    {
        // our socket has data
        return recive(msg, max_size);
    }
    // our socket has no data
    errno = EAGAIN;
    return -1;
}



