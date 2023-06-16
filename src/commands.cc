#include "commands.h"
#include <algorithm>
#include <boost/json.hpp>
#include <boost/json/object.hpp>
#include <functional>
#include <iostream>
#include <stdexcept>

namespace json = boost::json;

void Commands::recieve()
{
    std::string command;
    while (true)
    {
        std::getline(std::cin, command);
        queue.push_back(command);
        if (is_shutdown || command == "q")
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

void Commands::log_handler(std::string const&) {
}

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
    }
}

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
    if (line_type == "MSG")
    {
        for (auto const& [track, track_type, handler_type] : message_tracks)
        {
            std::string response{};
            bool matched{ false };
            response += content["nick"].as_string();
            response += ": ";
            response += content["data"].as_string();
            switch (track_type)
            {
            case TrackType::name:
            {
                if (content["nick"].as_string() == track)
                {
                    response += " [track/name]";
                    matched = true;
                }
            }
            break;
            case TrackType::match:
            {
                if (line.find(track) != line.npos)
                {
                    response += " [track/match]";
                    matched = true;
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
                            response += " [track/emote]";
                            matched = true;
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
                            response += " [track/mention]";
                            matched = true;
                        }
                    }
                }
            }
            break;
            }
            if (matched)
            {
                get_handler(handler_type)(response);
            }
        }
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
