#if !defined(CLIENT_HPP)
#define CLIENT_HPP

#define BUFFERSIZE 1024
#include "../xmlhandler/xmlhandler.hpp" // i dont like this
#include <boost/asio.hpp>
#include <string>
#include <iostream>
#include <vector>

#define HEARTBEAT_FREQUENCY 90

class Client
{
public:
    Client(boost::asio::io_context& ioc) 
    :   ioContext_(ioc)
    ,   tcpSocket_(ioc)
    ,   resolver_(ioContext_)
    ,   tempBuffer_(BUFFERSIZE,0)
    ,   pmxTimer_(ioc)
    {
        pmxTimer_.expires_after(std::chrono::seconds(HEARTBEAT_FREQUENCY));
    }
    ~Client()
    {
        std::cout << "Client::~Client" << std::endl;
    }
    void inline start(){
        do_resolve();
    }
    void inline set_endpoint(const char *host, const char *port){
        host_ = std::string(host);
        port_ = std::string(port);
    }
    void inline set_xmlhandler(XmlHandler *x){
        xmlHandler_ = x;
    }

private:
    std::string host_;
    std::string port_;
    boost::asio::io_context& ioContext_;
    boost::asio::ip::tcp::resolver resolver_;
    std::vector<boost::asio::ip::tcp::endpoint> endpoints_;
    std::vector<boost::asio::ip::tcp::endpoint>::iterator curEndpoint_;
    boost::asio::ip::tcp::socket tcpSocket_;
    std::vector<char> tempBuffer_; // this will be touched by the TCP socket
    void do_resolve();
    void do_connect();
    void do_read();
    boost::asio::high_resolution_timer pmxTimer_;
    XmlHandler *xmlHandler_;
    void inline reconnect()
    {
        printf("Reconnecting to %s:%s\n",host_.c_str(),port_.c_str());
        boost::system::error_code ec;
        tcpSocket_.cancel(ec);
        if(ec) printf("CANCEL ERROR: %s\n",ec.what().c_str());
        tcpSocket_.shutdown(boost::asio::ip::tcp::socket::shutdown_type::shutdown_both,ec);
        if(ec) printf("SHUTDOWN ERROR: %s\n",ec.what().c_str());
        tcpSocket_.close(ec);
        if(ec) printf("CLOSE ERROR: %s\n",ec.what().c_str());
        do_resolve();
    }
};

#endif // CLIENT_HPP
