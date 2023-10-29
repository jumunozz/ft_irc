#include "ft_irc.hpp"

int main()
{
    //Initialize winsock
    WSADATA wsDate;
    WORD ver = MAKEWORD(2, 2);

    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0)
    {
        std::cerr << "Can't Initialize winsock! Quitting" << std::endl;
        return -1;
    }
    //Create a socket
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET)
    {
        std::cerr << "Can't create a socket! Quitting" << std::endl;
        return -2;
    }
    //Bind the socket to an IP / port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000); //HostToNetworkShort does a translation of bits
    hint.sin_addr.S_un.S_addr = INADDR_ANY; //Could also use inet_pton...

    bind(listening, (sockaddr*)&hint, sizeof(hint)); //bind listening(the socket), using AF_INET(the format), to hint(the structure)
    
    //Tell Winsock the socket is for listening
    listen(listening, SOMAXCONN);

    fd_set master;

    FD_ZERO(&master);

    FD_SET(listening, &master);

    while (true)
    {
        fd_set  copy = master;

        int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

        for (int a = 0; a < socketCount; a++)
        {
            SOCKET sock = copy.fd_array[a];
            if (socket == listening)
            {
                //Accept a new connection
                SOCKET client = accept(listening, nullptr, nullptr);

                //Add the new connection to the list of connected clients
                FD_SET(client, &master);

                //Send a welcome message to the connected client
                std::string welcomeMsg = "Welcome to the Awesome Chat Server!" << std::endl;
                send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
            }
            else
            {
                char    buf[4096];
                ZeroMemory(buf, 4096);

                //Receive message
                int bytesIn = recv(sock, buf, 4096, 0);
                if (bytesIn <= 0)
                {
                    //Drop the client
                    closesocket(sock);
                    FD_CLR(sock, &master);
                }
                else
                {
                    //Send message to other clients, and definitely NOT the listening socket

                    for (int b = 0; b < master.fd_count; b++)
                    {
                        SOCKET outSock = master.fd_array[b];
                        if (outSock != listening && outSock != sock)
                        {
                            std::ostringstream ss;
                            ss < "SOCKET #" << sock << ": " << buf << std::endl;
                            std::string strOut = ss.str();

                            send(outSock, strOut.c_str(), strOut.size() + 1, 0);
                        }
                    }
                }
            }
        }
    }

    //Cleanup winsock
    WSACleanup();

    system("pause");
    
    return 0;
}

    /*//Wait for a connection
    sockaddr_in client;
    int clientSize = sizeof(client);

    SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
    
    char    host[NI_MAXHOST]; //1025 client's remote name
    char    svc[NI_MAXSERV]; //32 Service (i.e. port) the client is connected on

    ZeroMemory(host, NI_MAXHOST);
    ZeroMemory(svc, NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
    
    if (result)
    {
        std::cout << host << " connected on port " << svc << std::endl;
    }
    else
    {
        inet_ntop(AF_INET, &clinet.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
    }

    //Close the listening socket
    closesocket(listening);

    //While receiving display message, echo message
    char    buf[4096];

    while (true)
    {
        //Clear the buffer
        memset(buf, 0, 4096);
        //Wait for a message
        int bytesRecv = recv(clientSocket, buf, 4096, 0);
        if (bytesRecv == SOCKET_ERROR)
        {
            std::cerr << "There was a connection issue" << std::endl;
            break ;
        }
        if (bytesRecv == 0)
        {
            std::cout << "The client disconnected" << std::endl;
            break ;
        }
        //Display message
        std::cout << "Received: " << std::string(buf, 0, bytesRecv) << std::endl;

        //Resend message
        send(clientSocket, buf, bytesRecv + 1, 0);
    }*/