#include "commands.h"
#include "websocket.h"

#include <chrono>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>

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
            bool valid;

            // "quit" or the type of handler for the match being created
            oss >> line;
            if (line == "quit")
            {
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
