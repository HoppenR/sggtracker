#include "websocket.h"

#include <boost/asio/buffer.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <cstdlib>
#include <iostream>
#include <openssl/tls1.h>
#include <ostream>
#include <string>

namespace ip = boost::asio::ip;
namespace ssl = boost::asio::ssl;
namespace beast = boost::beast;
namespace net = boost::beast::net;

WebSocket::WebSocket(char const* host, char const* port, char const* target)
    : ctx{ ssl::context::tlsv12_client }, resolver{ io_ctx },
      wss{ io_ctx, ctx }, host{ host }, target{ target }
{
    ip::basic_resolver_results<ip::tcp> dom_res{ resolver.resolve(host, port) };
    net::connect(beast::get_lowest_layer(wss), dom_res);
    SSL_set_tlsext_host_name(wss.next_layer().native_handle(), host);
    wss.next_layer().handshake(ssl::stream_base::client);
    wss.handshake(host, target);
}

void WebSocket::open()
{
    wss.next_layer().handshake(ssl::stream_base::client);
    wss.handshake(host, target);
}

void WebSocket::recieve()
{
    std::string line{};
    auto buf{ boost::asio::dynamic_buffer(line) };
    while (true)
    {
        if (!wss.is_open())
        {
            std::cout << "attempting to reconnect" << std::endl;
            open();
        }
        wss.read(buf);
        queue.push_back(std::move(line));
        buf.consume(buf.size());
        if (is_shutdown)
        {
            return;
        }
    }
}

WebSocket::~WebSocket() { wss.close(beast::websocket::close_code::normal); }
