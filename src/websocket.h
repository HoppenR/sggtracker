#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "client.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <chrono>
#include <deque>
#include <string>

class WebSocket : public Client
{
public:
    WebSocket(char const*, char const*, char const*, boost::asio::io_context&);
    ~WebSocket();

    void recieve() override;

private:
    void open();

    using _socket = boost::asio::ip::tcp::socket;
    using _ssl_stream = boost::beast::ssl_stream<_socket>;
    using _websocket_stream = boost::beast::websocket::stream<_ssl_stream>;

    boost::asio::ssl::context ssl_ctx;
    boost::asio::ip::tcp::resolver resolver;
    std::string line;
    boost::asio::dynamic_string_buffer<char, std::char_traits<char>,
                                       std::allocator<char>>
        buf;

    _websocket_stream wss;
    std::string host;
    std::string target;
};

#endif // WEBSOCKET_H
