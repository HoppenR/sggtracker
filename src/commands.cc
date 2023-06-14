#include "commands.h"
#include <algorithm>
#include <boost/json.hpp>
#include <boost/json/object.hpp>
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

void Commands::track(std::string& target, TrackType type)
{
    message_tracks[std::move(target)] = type;
}

void Commands::handle_line(std::string const& line)
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
        for (auto const& [track, track_type] : message_tracks)
        {
            switch (track_type)
            {
            case TrackType::name:
            {
                if (content["nick"].as_string() == track)
                {
                    std::cout << content_str << " [track/name]\n";
                }
            }
            break;
            case TrackType::match:
            {
                if (content["data"].as_string() == track)
                {
                }
                if (line.find(track) != line.npos)
                {
                    std::cout << content_str << " [track/match]\n";
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
                            std::cout << content_str << " [track/emote]\n";
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
                            std::cout << content_str << " [track/mention]\n";
                        }
                    }
                }
            }
            break;
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
