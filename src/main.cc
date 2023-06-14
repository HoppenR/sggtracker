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
            oss >> line;

            if (line == "q")
            {
                break;
            }
            else if (line == "track")
            {
                oss >> line;
                if (line == "name")
                {
                    oss >> line;
                    std::cout << "tracking name: " << line << std::endl;
                    cmd.track(line, TrackType::name);
                }
                else if (line == "match")
                {
                    oss >> line;
                    std::cout << "tracking match: " << line << std::endl;
                    cmd.track(line, TrackType::match);
                }
                else if (line == "emote")
                {
                    oss >> line;
                    std::cout << "tracking emote: " << line << std::endl;
                    cmd.track(line, TrackType::emote);
                }
                else if (line == "mention")
                {
                    oss >> line;
                    std::cout << "tracking mention: " << line << std::endl;
                    cmd.track(line, TrackType::mention);
                }
            }
        }
        if (wss.getline(line))
        {
            cmd.handle_line(line);
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
