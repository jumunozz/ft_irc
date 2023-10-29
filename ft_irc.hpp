#pragma once

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <sstream>
#include <map>

std::string to_string(int nb);

class Users
{
private:
    std::string   userName;
    std::string   nickName;
    int           socketNum;
    int           maxAttempts;
    bool          checkAuth;
    bool          isOperator();
public:
    Users();
    Users(const int& socket);
    ~Users();
    Users& operator=(const Users& other);
    std::string getUserName() const;
    std::string getNickName() const;
    int         getSocketNum() const;
    int         getMaxAttempts() const;
    bool        getCheckAuth() const;
    void        setUserName(const std::string& newUserName);
    void        setNickName(const std::string& newNickName);
    void        setSocketNum(const int& newSocketNum);
    void        setMaxAttempts(const int& newMaxAttempts);
    void        setCheckAuth(bool newCheckAuth);
};

class Channels
{
private:
    const std::string   chanName;
    bool                hasClients();
public:
    Channels();
    Channels(const int& socket);
    ~Channels();
    Channels& operator=(const Channels& other);
    std::string getChanName() const;
    void        setChanName(const std::string& newChanName);
};