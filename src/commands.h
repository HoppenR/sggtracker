#ifndef COMMANDS_H
#define COMMANDS_H

#include "client.h"

#include <iostream>
#include <map>
#include <sstream>

enum class TrackType
{
    name,
    match,
    emote,
    mention,
};

class Commands : public Client
{
public:
    Commands() = default;
    ~Commands() = default;

    void recieve() override;
    void track(std::string&, TrackType);
    void handle_line(std::string const&);

private:
    std::map<std::string, TrackType> message_tracks{};
};

#endif // COMMANDS_H
