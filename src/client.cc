#include "client.h"
#include <algorithm>

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

void Client::set_shutdown() { is_shutdown = true; }
