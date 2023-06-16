#include "commands.h"
#include "websocket.h"

#include <chrono>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

void event_loop()
{
    Commands cmd{};
    WebSocket wss{ "chat.strims.gg", "443", "/ws" };
    std::thread cmd_thrd{ [&cmd] { cmd.recieve(); } };
    std::thread wss_thrd{ [&wss] { wss.recieve(); } };

    std::string line;
    while (true)
    {
        if (cmd.getline(line))
        {
            std::istringstream oss{ line };

            // "quit" or the type of handler for the match being created
            oss >> line;
            HandlerType handler;

            if (line == "quit")
            {
                break;
            }

            if (line == "print")
            {
                handler = HandlerType::print;
            }
            else if (line == "log")
            {
                handler = HandlerType::log;
            }
            else
            {
                continue;
            }

            // type of data to match against
            oss >> line;
            TrackType track_type;

            if (line == "name")
            {
                track_type = TrackType::name;
            }
            else if (line == "match")
            {
                track_type = TrackType::match;
            }
            else if (line == "emote")
            {
                track_type = TrackType::emote;
            }
            else if (line == "mention")
            {
                track_type = TrackType::mention;
            }
            else
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
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::cout << "wait WebSocket::wss::read() to exit" << std::endl;
    cmd.set_shutdown();
    wss.set_shutdown();
    cmd_thrd.join();
    wss_thrd.join();
}

int main()
{
    event_loop();
    std::cout << "exit safetly" << std::endl;
}
