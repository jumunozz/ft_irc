#include "ft_irc.hpp"

Users::Users() {}

Users::Users(const int& socket) : socketNum(socket) {}

Users::~Users() {}

Users& Users::operator=(const Users& other)
{
    if (this != &other)
    {
        userName = other.userName;
        nickName = other.nickName;
        socketNum = other.socketNum;
        maxAttempts = other.maxAttempts;
    }
    return *this;
}

std::string Users::getUserName() const
{
    return userName;
}

std::string Users::getNickName() const
{
    return nickName;
}

int Users::getSocketNum() const
{
    return socketNum;
}

int Users::getMaxAttempts() const
{
    return maxAttempts;
}

int Users::getCheckAuth() const
{
    return  checkAuth;
}

void    Users::setUserName(const std::string& newUserName)
{
    userName = newUserName;
}

void    Users::setNickName(const std::string& newNickName)
{
    nickName = newNickName;
}

void    Users::setSocketNum(const int& newSocketNum)
{
    socketNum = newSocketNum;
}

void    Users::setMaxAttempts(const int& newMaxAttempts)
{
    maxAttempts = newMaxAttempts;
}

bool    Users::setCheckAuth(const int& newCheckAuth)
{
    checkAuth = newCheckAuth;
}