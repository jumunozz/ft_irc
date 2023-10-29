#include "ft_irc.hpp"

int main()
{
    //Create a socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        std::cerr << "Can't create a socket." << std::endl;
        return -1;
    }
    //Bind the socket to an IP / port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000); //HostToNetworkShort does a translation of bits
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr); //internet command, pointer to string to a number, converts a number to an array of integer

    if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1) //bind listening(the socket), using AF_INET(the format), to hint(the structure)
    {
        std::cerr << "Can't bind to IP/port" << std::endl;
        return -2;
    }
    //Mark the socket for listening in
    if (listen(listening, SOMAXCONN) == -1) //SOMAXCONN = maximum number of connexion
    {
        std::cerr << "Can't listen." << std::endl;
        return -3;
    }
    //Accept a call
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char    host[NI_MAXHOST]; //1025
    char    svc[NI_MAXSERV]; //32

    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    if (clientSocket == -1)
    {
        std::cerr << "Problem with client connecting." << std::endl;
        return -4;
    }
    //Close the listening socket
    close(listening);

    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);

    if (result)
        std::cout << host << " connected on " << svc << std::endl;
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
    }
    //While receiving display message, echo message
    char    buf[4096];
    while (true)
    {
        //Clear the buffer
        memset(buf, 0, 4096);
        //Wait for a message
        int bytesRecv = recv(clientSocket, buf, 4096, 0);
        if (bytesRecv == -1)
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
    }
    //Close Socket
    close(clientSocket);
    
    return 0;
}