#ifndef CLIENT_H
#define CLIENT_H

#include <boost/asio/io_context.hpp>
#include <deque>
#include <string>

class Client
{
public:
    Client(boost::asio::io_context&);
    virtual ~Client() = default;

    virtual void recieve() = 0;
    bool getline(std::string& line);

protected:
    std::deque<std::string> queue{};
    boost::asio::io_context& io_ctx;
};

#endif // CLIENT_H
