#ifndef COMMANDS_H
#define COMMANDS_H

#include "client.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/streambuf.hpp>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

class TrackType
{
public:
    enum Value
    {
        name,
        match,
        emote,
        mention,
    };
    TrackType() = default;
    TrackType(Value v) : val{ v } {}
    static std::pair<TrackType, bool> from_str(std::string&);
    operator Value() { return val; };

private:
    Value val;
};

class HandlerType
{
public:
    enum Value
    {
        count,
        print,
        log
    };
    HandlerType() = default;
    HandlerType(Value v) : val{ v } {}
    static std::pair<HandlerType, bool> from_str(std::string&);
    operator Value() { return val; }

private:
    Value val;
};

class Commands : public Client
{
public:
    Commands(boost::asio::io_context&);
    ~Commands();

    void recieve() override;
    void add_tracker(std::string&, TrackType, HandlerType);
    void parse_line(std::string const&);
    std::function<void(std::string const&)> get_handler(HandlerType) const;

private:
    std::vector<std::tuple<std::string, TrackType, HandlerType>>
        message_tracks{};
    static std::map<std::string, int> counters;
    static void count_handler(std::string const&);
    static void print_handler(std::string const&);
    static void log_handler(std::string const&);

    boost::asio::posix::stream_descriptor stdin_stream;
    boost::asio::streambuf buf;
};

#endif // COMMANDS_H
