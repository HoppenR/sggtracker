#ifndef COMMANDS_H
#define COMMANDS_H

#include "client.h"

#include <functional>
#include <iostream>
#include <sstream>
#include <tuple>
#include <vector>

enum class TrackType
{
    name,
    match,
    emote,
    mention,
};

enum class HandlerType
{
    print,
    log,
};

class Commands : public Client
{
public:
    Commands() = default;
    ~Commands() = default;

    void recieve() override;
    void add_tracker(std::string&, TrackType, HandlerType);
    void parse_line(std::string const&);
    std::function<void(std::string const&)> get_handler(HandlerType) const;

private:
    std::vector<std::tuple<std::string, TrackType, HandlerType>>
        message_tracks{};
    static void print_handler(std::string const&);
    static void log_handler(std::string const&);
};

#endif // COMMANDS_H
