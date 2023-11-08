#include "websocket.h"

#include <boost/asio/buffer.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <openssl/tls1.h>
#include <ostream>
#include <string>
#include <thread>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace ip = boost::asio::ip;
namespace net = boost::beast::net;
namespace ssl = boost::asio::ssl;
namespace websocket = boost::beast::websocket;

using namespace std::chrono_literals;

WebSocket::WebSocket(char const* host, char const* port, char const* target,
                     asio::io_context& ctx)
    : Client{ ctx }, ssl_ctx{ ssl::context::tlsv12_client }, resolver{ io_ctx },
      buf{ asio::dynamic_buffer(line) }, wss{ io_ctx, ssl_ctx }, host{ host },
      target{ target }
{
    ip::basic_resolver_results<ip::tcp> dom_res{ resolver.resolve(host, port) };

    wss.set_option(websocket::stream_base::timeout{
        /* .handshake_timeout = */ websocket::stream_base::none(),
        /* .idle_timeout = */ std::chrono::seconds(15),
        /* .keep_alive_pings = */ true,
    });

    net::connect(beast::get_lowest_layer(wss), dom_res);
    SSL_set_tlsext_host_name(wss.next_layer().native_handle(), host);
    this->open();
}

WebSocket::~WebSocket()
{
    wss.close(beast::websocket::close_code::normal);
    std::cerr << "websocket closed safetly" << std::endl;
}

void WebSocket::open()
{
    try
    {
        std::cerr << "Attempting to connect..." << std::endl;
        wss.next_layer().handshake(ssl::stream_base::client);
        wss.handshake(host, target);
    }
    catch (const boost::system::system_error& err)
    {
        std::cerr << "Error connecting:" << err.what() << std::endl;
    }
}

void WebSocket::recieve()
{
    wss.async_read(
        buf,
        [this](boost::system::error_code error_code, std::size_t)
        {
            if (!error_code)
            {
                queue.push_back(line);
                buf.consume(buf.size());
                if (io_ctx.stopped())
                {
                    return;
                }

                io_ctx.post([this]() { this->recieve(); });
            }
            else if (error_code == boost::asio::error::connection_reset)
            {
                std::cerr << "Connection reset..." << std::endl;
                // this->open();
                // io_ctx.post([this]() { this->recieve(); });
                io_ctx.stop();
            }
            else if (error_code == websocket::error::closed)
            {
                std::cerr << "Websocket closed..." << std::endl;
                // this->open();
                // io_ctx.post([this]() { this->recieve(); });
                io_ctx.stop();
            }
            else if (error_code == beast::error::timeout)
            {
                std::cerr << "Websocket connectio timed out..." << std::endl;
                // this->open();
                // io_ctx.post([this]() { this->recieve(); });
                io_ctx.stop();
            }
            // else if (error_code ==
            // boost::beast::http::error::end_of_stream)
            // {
            //     std::cerr << "Websocket closed..." << std::endl;
            //     this->open();
            //     io_ctx.post([this]() { this->recieve(); });
            // }
            else
            {
                std::cerr << "Error reading websocket: " << error_code.message()
                          << std::endl;
                io_ctx.stop();
            }
        });
}
