#ifndef CLIENT_H
#define CLIENT_H

#include <deque>
#include <string>

class Client
{
public:
    virtual ~Client() = default;

    virtual void recieve() = 0;
    bool getline(std::string& line);
    void set_shutdown();

protected:
    std::deque<std::string> queue{};
    bool is_shutdown{ false };
};

#endif // CLIENT_H
