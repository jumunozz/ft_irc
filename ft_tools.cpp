#include "ft_irc.hpp"

std::string to_string(int nb)
{
    std::ostringstream oss;
    oss << nb;
    return (std::string(oss.str()));
}