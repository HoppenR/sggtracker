#include "commands.h"
#include <algorithm>
#include <boost/json.hpp>
#include <boost/json/object.hpp>
#include <functional>
#include <iostream>
#include <map>
#include <stdexcept>

namespace json = boost::json;

void Commands::recieve()
{
    std::string command;
    while (true)
    {
        std::getline(std::cin, command);
        queue.push_back(command);
        if (is_shutdown || command == "quit")
        {
            return;
        }
    }
}

void Commands::add_tracker(std::string& target, TrackType type,
                           HandlerType handler)
{
    message_tracks.emplace_back(std::move(target), type, handler);
}

void Commands::print_handler(std::string const& data)
{
    std::cout << data << std::endl;
}

void Commands::count_handler(std::string const& data)
{
    counters[data]++;
    std::cout << counters[data] << " counts of " << data << std::endl;
}

void Commands::log_handler(std::string const&) {}

std::function<void(std::string const&)>
Commands::get_handler(HandlerType handler) const
{
    switch (handler)
    {
    case HandlerType::print:
    {
        return Commands::print_handler;
    }
    break;
    case HandlerType::log:
    {
        return Commands::log_handler;
    }
    break;
    case HandlerType::count:
    {
        return Commands::count_handler;
    }
    break;
    }
    throw std::logic_error{ "unreachable" };
}

// TODO: make the dispatch to the callbacks give a struct with name, content
//       instead of sending a pure string, then callbacks can construct/use
//       whatever they need to
void Commands::parse_line(std::string const& line)
{
    auto space_pos{ std::find(line.begin(), line.end(), ' ') };
    if (space_pos == line.end())
    {
        return;
    }
    std::string const line_type{ line.begin(), space_pos };
    std::string const content_str{ std::next(space_pos), line.end() };
    json::object content{ json::parse(content_str).as_object() };
    std::map<HandlerType::Value, std::string> dispatch{};
    if (line_type == "MSG")
    {
        std::string response{};
        response += content["nick"].as_string();
        response += ": ";
        response += content["data"].as_string();
        for (auto& [track, track_type, handler_type] : message_tracks)
        {
            switch (track_type)
            {
            case TrackType::name:
            {
                if (content["nick"].as_string() == track)
                {
                    dispatch[handler_type] += " [track/name]";
                }
            }
            break;
            case TrackType::match:
            {
                if (line.find(' ' + track + ' ') != line.npos)
                {
                    dispatch[handler_type] += " [track/match]";
                }
            }
            break;
            case TrackType::emote:
            {
                if (content["entities"].as_object().contains("emotes"))
                {
                    json::array const& emote_array{
                        content["entities"].as_object()["emotes"].as_array()
                    };
                    for (json::value emote_entry : emote_array)
                    {
                        if (emote_entry.as_object()["name"].as_string() ==
                            track)
                        {
                            dispatch[handler_type] += " [track/emote]";
                        }
                    }
                }
            }
            break;
            case TrackType::mention:
            {
                if (content["entities"].as_object().contains("nicks"))
                {
                    json::array const& nick_array{
                        content["entities"].as_object()["nicks"].as_array()
                    };
                    for (json::value nick_entry : nick_array)
                    {
                        if (nick_entry.as_object()["nick"].as_string() == track)
                        {
                            dispatch[handler_type] += " [track/mention]";
                        }
                    }
                }
            }
            break;
            }
        }
        for (auto const& [handler_type, tracks] : dispatch)
        {
            get_handler(handler_type)(response + tracks);
        }
    }
    else if (line_type == "MUTE")
    {
    }
    else if (line_type == "JOIN")
    {
    }
    else if (line_type == "QUIT")
    {
    }
    else if (line_type == "VIEWERSTATE")
    {
    }
    else if (line_type == "NAMES")
    {
        std::cout << "connected, " << content["connectioncount"].as_int64()
                  << " connections" << std::endl;
    }
    else
    {
        throw std::runtime_error{ "unrecognized type: " + line_type };
    }
}

std::pair<HandlerType, bool> HandlerType::from_str(std::string& line)
{
    if (line == "count")
    {
        return { HandlerType::count, true };
    }
    else if (line == "print")
    {
        return { HandlerType::print, true };
    }
    else if (line == "log")
    {
        return { HandlerType::log, true };
    }
    return { {}, false };
}

std::pair<TrackType, bool> TrackType::from_str(std::string& line)
{
    if (line == "name")
    {
        return { TrackType::name, true };
    }
    else if (line == "match")
    {
        return { TrackType::match, true };
    }
    else if (line == "emote")
    {
        return { TrackType::emote, true };
    }
    else if (line == "mention")
    {
        return { TrackType::mention, true };
    }
    return { {}, false };
}

std::map<std::string, int> Commands::counters{};
