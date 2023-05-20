#include "client.hpp"

void Client::do_resolve()
{
    resolver_.async_resolve
    (
        host_,port_,
        [this](const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator endpoint_iter)
        {
            if(ec)
            {
                std::cout << "RESOLVE ERROR " << ec.what() << std::endl;
                do_resolve();
            }
            else
            {
                for (; endpoint_iter != boost::asio::ip::tcp::resolver::iterator(); ++endpoint_iter) {
                    endpoints_.push_back(*endpoint_iter);
                }
                curEndpoint_ = endpoints_.begin();
                do_connect();
            }
        }
    );
}

void Client::do_connect()
{
    auto ep = *curEndpoint_;
    tcpSocket_.async_connect
    (
        ep,
        [this](const boost::system::error_code& ec)
        {
            if(!ec)
            {
                do_read();
            }
            else
            {
                std::cout << "CONNECT ERROR " << ec.what() << std::endl;
                std::cout << "NEXT ENDPOINT" << std::endl;
                boost::system::error_code ec2;
                if(tcpSocket_.is_open())
                {
                    tcpSocket_.cancel(ec2);
                    if(ec2) std::cout << "CANCEL ERROR " << ec.what() << std::endl;
                    tcpSocket_.close(ec2);
                    if(ec2) std::cout << "CLOSE ERROR " << ec.what() << std::endl;
                }
                curEndpoint_++;
                if(curEndpoint_ == endpoints_.end())
                {
                    endpoints_.clear();
                    boost::asio::steady_timer timer(ioContext_, std::chrono::seconds(1));
                    timer.wait();
                    do_resolve();
                }
                else
                    do_connect();
            }
            
        }
    );
}

void Client::do_read()
{
    pmxTimer_.expires_after(std::chrono::seconds(HEARTBEAT_FREQUENCY));
    pmxTimer_.async_wait
    (
        [this](const boost::system::error_code ec)
        {
            if(!ec)
            {
                std::cout << "TIMER EXPIRED" << host_ << std::endl;
                xmlHandler_->clear_history();
                reconnect();
            }
            if(ec == boost::asio::error::operation_aborted)
            {
                // std::cout << "TIMER RESET " << host_ << std::endl;
            }
            if(ec && ec != boost::asio::error::operation_aborted)
            {
                std::cout << "TIMER ERROR " << host_ << ec.what() << std::endl;
            }
        }
    );
    
    tcpSocket_.async_receive
    (
        boost::asio::buffer(tempBuffer_),
        [&](boost::system::error_code ec, size_t size)
        {
            if(ec)
            {
                std::cout << "ASYNC_RECEIVE ERROR " << ec.what().c_str() << std::endl;
                return;
            }
            else
            {
                // std::cout << "Concating " << host_ << std::endl;
                auto end = tempBuffer_.begin() + size; // std::next 
                xmlHandler_->append(host_,std::move(tempBuffer_),size);
            }
            do_read();
        }
    );
}


