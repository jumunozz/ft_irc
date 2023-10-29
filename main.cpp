#include "ft_irc.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage : " << argv[0] << " <port> <password>" << std::endl;
        return -5;
    }

    int port = atoi(argv[1]);
    char *pwd = argv[2];

    //Create a socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);

    if (listening == -1)
    {
        std::cerr << "Can't create a socket ! Quitting" << std::endl;
        return -1;
    }

    //Bind the socket to an IP / port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    hint.sin_addr.s_addr = INADDR_ANY;

    if (bind(listening, reinterpret_cast<struct sockaddr*>(&hint), sizeof(hint)) == -1)
    {
        std::cerr << "Can't bind to IP/Port ! Quitting" << std::endl;
        return -2;
    }

    //Tell the socket to listen
    if (listen(listening, SOMAXCONN) == -1)
    {
        std::cerr << "Can't listen on socket ! Quitting" << std::endl;
        return -3;
    }
    
    fd_set  master;
    FD_ZERO(&master);
    FD_SET(listening, &master);

    int maxSocket = listening;

    std::map<int, Users> clientsInfo; //Map to store client socket descriptors and their state (check, maxAttempts)

    std::cout << "Server is listening." << std::endl;

    while (true)
    {
        fd_set copy = master;

        int socketCount = select(maxSocket + 1, &copy, NULL, NULL, NULL);

        if (socketCount == -1)
        {
            std::cerr << "Error with select ! Quitting" << std::endl;
            return -4;
        }
        for (int sock = 0; sock <= maxSocket; sock++)
        {
            if (FD_ISSET(sock, &copy))
            {
                if (sock == listening)
                {
                    //Accept a new connection
                    int client = accept(listening, NULL, NULL);
                    if (client == -1)
                    {
                        std::cerr << "Couldn't accept client ! Quitting" << std::endl;
                        return -7;
                    }
                    else
                        clientsInfo[sock] = Users(sock);

                    //Add the new connection to the list of connected clients
                    FD_SET(client, &master);
                    if (client > maxSocket)
                        maxSocket = client;
                    
                    //Send a welcome message to the connected client
                    std::string pingMsg = "PING :server.louis\n";
                    send(client, pingMsg.c_str(), pingMsg.size(), 0);
                    
                    //Send an authentication request
                //    std::string authReq = "Please enter the password : ";
                //    send(client, authReq.c_str(), authReq.size(), 0);
                }
                else
                {
                    char    buf[4096];
                    memset(buf, 0, sizeof(buf));

                    //Receive message
                    int bytesIn = recv(sock, buf, sizeof(buf), 0);
                    std::cout << "|Buf is : " << buf << "|" << std::endl;
                    if (bytesIn <= 0)
                    {
                        close(sock);
                        FD_CLR(sock, &master);
                    }
                    else
                    {
                        //Send message to other clients, and definitely NOT the listening socket
                        for (int outSock = 0; outSock <= maxSocket; outSock++)
                        {
                            if (FD_ISSET(outSock, &master) && outSock != listening && outSock != sock)
                            {
                                std::ostringstream ss;
                                ss << "SOCKET #" << sock << ": " << buf << std::endl;
                                std::string strOut = ss.str();
                                send(outSock, strOut.c_str(), strOut.size(), 0);
                            }
                        }
                        if (strncmp(buf, "CAP LS", 6) == 0)
                        {
                            //Handle CAP LS
                            std::cout << "Received CAP LS" << std::endl;
                            std::string capLs = ":server.louis CAP * LS :NICK USER JOIN MSG KICK INVITE TOPIC MODE\n";
                            send(sock, capLs.c_str(), capLs.size(), 0);
                        }
                        /*else if (strncmp(buf, "JOIN :", 6) == 0)
                        {
                            //Handle JOIN command
                            std::string channelName = buf + 7;

                            //Check if the channel name is empty or contains only whitespace
                            bool isWhitespace = true;
                            bool validChanname = true;
                            for (std::string::iterator it = channelName.begin(); it != channelName.end(); ++it)
                            {
                                if (channelName.begin != '#')
                                {
                                    validChanname = false;
                                    break;
                                }
                                if (!std::isspace(*it))
                                {
                                    isWhitespace = false;
                                    break;
                                }
                            }
                            if (channelName.empty() || isWhitespace)
                            {
                                //Respond with an error message to the client
                                std::string errorMsg = "Error: You must specify a valid channel name to join.\n";
                                send(sock, errorMsg.c_str(), errorMsg.size(), 0);
                            }
                            else if (validChanname)
                            {
                                std::string chanInvalid = "Error: The channel name must start by #.\n";
                                send(sock, chanInvalid.c_str(), chanInvalid.size(), 0);
                            }
                            else
                            {
                                //Join the specified channel or default channel
                                std::string channelMsg = "Joined channel " + channelName + "\n";
                                send(sock, channelMsg.c_str(), channelMsg.size(), 0);
                            }
                        }*/
                        else if (strncmp(buf, "PONG", 4) == 0)
                        {
                            clientsInfo[sock].setCheckAuth(0);
                            clientsInfo[sock].setMaxAttempts(3);
                            std::string authReq = "Please enter the password : ";
                            send(sock, authReq.c_str(), authReq.size(), 0);
                            bool serverAuth = clientsInfo[sock].isAuthenticated(clientsInfo, buf, pwd, sock);
                            while (clientsInfo[sock].getMaxAttempts() > 0)
                            {
                                if (serverAuth)
                                {    
                                    std::string welcomeMsg = "#You are now connected to the server IRC :)\n#NICK <nickname> - Set/change your nickname.\n#USER <username> - Set/change your username.\n#JOIN <#channelname> - Create/join channel.\n#MSG <nickname> <your message> - Send a private message to the person.\n#MSG <#channelname> <your message> - Send a message to the channel.\n#If you are a channel Operator you can :\n#KICK <#channelname> <nickname,nickname> <reason> - Eject the user/s from channel. If no channel is given the active channel will be used.\n#INVITE <nickname> <#channelname> - Invite a user to a channel. If no channel is given the active channel will be used.\n#TOPIC <#channelname> - Display the topic.\n#TOPIC <#channelname> <topic> - Change the channel topic.\n#MODE - Change the channel mode :\n# +i set Invite-only channel.\n# -i remove Invite-only channel.\n# +t set the restrictions of the topic command to channel operators.\n# -t remove the restrictions of the topic command to channel operators.\n# +k <key> set the channel key (password).\n# -k <key> remove the channel key (password).\n# +o <nickname> give channel operator privilege.\n# -o <nickname> take channel operator privilege.\n# +l <count> set the user limit to channel.\n# -l <count> remove the user limit to channel.\n";
                                    send(sock, welcomeMsg.c_str(), welcomeMsg.size(), 0);
                                }
                                else
                                {
                                    std::string wrongPwd = "Authentication failed. The password you entered is incorrect. You have " + to_string(clientsInfo[sock].getMaxAttempts()) + "attempts remaining : ";
                                    send(sock, wrongPwd.c_str(), wrongPwd.size(), 0);
                                    if (clientsInfo[sock].getMaxAttempts() == 1)
                                    {
                                        close(sock);
                                        FD_CLR(sock, &master);
                                    }
                                }
                            }
                        }
                        else if (strncmp(buf, "NICK", 4) == 0 || strncmp(buf, "nick", 4) == 0)
                        {
                            //Handle NICK command
                            //Example: NICK yournickname
                            std::string newNick = buf + 5;
                            clientsInfo[sock].setNickName(newNick);

                            //Respond to the client with a message confirming the nickname change
                            std::string nickResponse = "Your nickname is now " + clientsInfo[sock].getNickName() + "\n";
                            send(sock, nickResponse.c_str(), nickResponse.size(), 0);
                        }
                        else if (strncmp(buf, "USER", 4) == 0 || strncmp(buf, "user", 4) == 0)
                        {
                            //Handle USER command
                            //Example: USER yourusername
                            std::string newUser = buf + 5;
                            clientsInfo[sock].setUserName(newUser);

                            //Respond to the client with a message confirming the username change
                            std::string userResponse = "Your username is now " + clientsInfo[sock].getUserName() + "\n";
                            send(sock, userResponse.c_str(), userResponse.size(), 0);
                        }
                    }
                    /*else
                    {
                        int& check = clientState[sock].first;
                       // int& maxAttempts = clientState[sock].second;
                        if (check == 0)
                        {
                            if (strncmp(buf, pwd, strlen(pwd)) == 0)
                            {
                                //Correct password
                                check = 1;
                                std::string goodPwd = "Authentication successful. You can now chat with other clients :)\n";
                                send(sock, goodPwd.c_str(), goodPwd.size(), 0);
                                break ;
                            }
                            else
                            {
                                //Incorrect password
                                maxAttempts--;
                                std::string wrongPwd = "Authentication failed. The password you entered is incorrect. You have " + to_string(maxAttempts) + " attempts remaining : ";
                                send(sock, wrongPwd.c_str(), wrongPwd.size(), 0);
                                if (maxAttempts == 0)
                                {
                                    close(sock);
                                    FD_CLR(sock, &master);
                                }
                            }
                        }
                    }*/
                }
            }
        }
    }
    return 0;
}