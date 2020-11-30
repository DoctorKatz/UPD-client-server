//
// Created by 79164 on 15.11.2020.
//

#ifndef UDP_SERVER_UDP_SERVER_H
#define UDP_SERVER_UDP_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdexcept>
#include <string.h>
#include <unistd.h>

namespace udp_server
{

    class udp_client_server_runtime_error : public std::runtime_error
    {
    public:
        udp_client_server_runtime_error(const char *w) : std::runtime_error(w) {}
    };


   /* class udp_client
    {
    public:
        udp_client(const std::string& addr, int port);
        ~udp_client();

        int                 get_socket() const;
        int                 get_port() const;
        std::string         get_addr() const;

        int                 send(const char *msg, size_t size);

    private:
        int                 f_socket;
        int                 f_port;
        std::string         f_addr;
        struct addrinfo *   f_addrinfo;
    };*/

   class UdpServer
    {
    public:
        UdpServer(int port);
        ~UdpServer();

        int                 get_socket() const;
        int                 get_port() const;
        std::string         get_addr() const;

        int                 recive(char *msg, size_t max_size);
        int                 send(const char *msg, size_t size);
        int                 timed_recv(char *msg, size_t max_size, int max_wait_ms);

    private:
       int                 f_socket;
       int                 f_port;
       std::string         f_addr;
       //struct addrinfo *   f_addrinfo;
       struct sockaddr_in  client_addr;
       struct sockaddr_in   server_addr;


    };

} // namespace udp_client_server

#endif //UDP_SERVER_UDP_SERVER_H
