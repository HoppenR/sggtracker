#include "util.h"

std::string join_vector(std::vector<std::string> const& string_vector)
{
    std::stringstream joinedString;
    for (size_t i = 0; i < string_vector.size(); ++i)
    {
        if (i > 0)
        {
            joinedString << ",";
        }
        joinedString << string_vector[i];
    }
    return joinedString.str();
}
