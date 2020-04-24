#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>

//windows
/*
#include <winsock.h>
#include <windows.h>
#include <ws2tcpip.h>
*/

int main()
{
    auto port_num = htons(54000);
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        std::cerr << "cannot create a socket" << std::endl;
        return -1;
    }

    //bind ip and port to sock
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = port_num;
    server.sin_addr.s_addr = INADDR_ANY;
    if (bind(listening, (sockaddr *) &server, sizeof(server)) == -1)
    {
        std::cerr << "cannot bind to IP/port" << std::endl;
        return -2;
    }

    if (listen(listening, SOMAXCONN) == -1)
    {
        std::cerr << "cannot listen" << std::endl;
        return -3;
    }

    //wait for connection
    sockaddr_in client;
    socklen_t client_size = sizeof(client);
	
	std::cout << "waiting for connection" << std::endl;
    int client_socket = accept(listening, (sockaddr *)&client, &client_size);
    if (client_socket == -1)
    {
        std::cerr << "client connecting problem" << std::endl;
        return -4;
    }

    //close(listening); // can be here, probably

    char host[NI_MAXHOST]; // client's remote name
    char service[NI_MAXSERV]; // port the client is connected on
    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);

    if (getnameinfo((sockaddr*)&client, client_size, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) // successful
        std::cout << host << " connected on port " << service << std::endl;
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on port " << ntohs(client.sin_port) << " getnameinfo unsuccessful" << std::endl;
    }
    //close listening socket to prevent from connecting with other clients
    close(listening);
    size_t buffer_size = 4096;
    char buffer[buffer_size];
    while (true)
    {
        memset(buffer, 0, buffer_size);
        int bytes_recv = recv(client_socket, buffer, buffer_size, 0);
        if (bytes_recv == -1)
        {
            std::cerr << "recv() error" << std::endl;
            break;
        }
        else if (bytes_recv == 0)
        {
            std::cout << "client disconnected" << std::endl;
            break;
        }

        std::cout << "received: " << std::string(buffer, 0, bytes_recv);// << std::endl;
        //echo mess
        send(client_socket, buffer, bytes_recv + 1, 0);
        
        
    }
    close(client_socket);
    return 0;
}

