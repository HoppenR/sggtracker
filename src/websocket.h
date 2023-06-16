#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "client.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <deque>
#include <string>

class WebSocket : public Client
{
public:
    WebSocket(char const*, char const*, char const*);
    ~WebSocket();

    void recieve() override;

private:
    void open();

    using _socket = boost::asio::ip::tcp::socket;
    using _ssl_stream = boost::beast::ssl_stream<_socket>;
    using _websocket_stream = boost::beast::websocket::stream<_ssl_stream>;

    boost::asio::io_context io_ctx;
    boost::asio::ssl::context ctx;
    boost::asio::ip::tcp::resolver resolver;

    _websocket_stream wss;
    std::string host;
    std::string target;
};

#endif // WEBSOCKET_H
