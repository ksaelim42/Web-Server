#ifndef INCLUDED_HTTP_TCPSERVER_LINUX
#define INCLUDED_HTTP_TCPSERVER_LINUX

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string>

namespace http
{
    class TcpServer
    {
    public:
        TcpServer();
        ~TcpServer();
    private:
        int m_port;
        int m_socket;
        int m_new_socket;
        long    m_incomingMessage;
        struct  sockaddr_in m_socketAddress;
        unsigned int    m_socketAddress_len;
        std::string m_serverMessag;
        
    };
} // namespace http
#endif