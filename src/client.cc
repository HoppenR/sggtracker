#include "client.h"
#include <algorithm>

namespace asio = boost::asio;

Client::Client(asio::io_context& ctx) : io_ctx{ ctx } {}

bool Client::getline(std::string& line)
{
    if (queue.size() > 0)
    {
        line = std::move(queue.front());
        queue.pop_front();
        return true;
    }
    return false;
}
