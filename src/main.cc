#include "commands.h"
#include "websocket.h"

#include "boost/asio/io_context.hpp"
#include <chrono>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>

namespace asio = boost::asio;

void event_loop()
{
    asio::io_context io_ctx{};

    Commands cmd{ io_ctx };
    WebSocket wss{ "chat.strims.gg", "443", "/ws", io_ctx };

    io_ctx.post([&cmd]() { cmd.recieve(); });
    io_ctx.post([&wss]() { wss.recieve(); });

    std::string line;
    while (!(io_ctx.stopped()))
    {
        io_ctx.run_one();
        if (cmd.getline(line))
        {
            std::istringstream oss{ line };
            bool valid;

            // "quit" or the type of handler for the match being created
            oss >> line;
            if (line == "quit")
            {
                io_ctx.stop();
                break;
            }
            HandlerType handler;
            std::tie(handler, valid) = HandlerType::from_str(line);
            if (!valid)
            {
                continue;
            }

            // type of data to match against
            oss >> line;
            TrackType track_type;
            std::tie(track_type, valid) = TrackType::from_str(line);
            if (!valid)
            {
                continue;
            }

            // query string
            oss >> line;
            cmd.add_tracker(line, track_type, handler);
        }

        if (wss.getline(line))
        {
            cmd.parse_line(line);
        }
    }
}

int main()
{
    /*
     * TODO: Maybe use namespaces or better naming for struct?
     *                ::Client,
     *       WebSocket::Client
     *        Commands::Client
     * TODO: Look into turning io_context posts into threads
     * TODO: Maybe move other structs from commands.h into new folder/file
     */
    event_loop();
}
