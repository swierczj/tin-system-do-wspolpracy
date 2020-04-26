#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <sys/ioctl.h>
#include "serv_func.h"

//windows
#include <winsock.h>
#include <windows.h>
#include <ws2tcpip.h>


int main()
{
    auto port_num = htons(54000);
    bool run_server = true;
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        std::cerr << "cannot create a socket" << std::endl;
        return -1;
    }

    // set listening socket to be reusable and nonblocking
    // opt_arg is an option argument passed to ioctl() and setsockopt()
    int opt_arg = 1;
    char* argp = (char*)&opt_arg;
    // set to reusable
    if(setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, argp, sizeof(opt_arg)) == -1)
    {
        std::cerr << "setsockopt() error" << std::endl;
        close(listening);
        return -2;
    }
    // nonblocking option on
    if(ioctl(listening, FIONBIO, argp) == -1)
    {
        std::cerr << "ioctl() error" << std::endl;
        close(listening);
        return -3;
    }

    //bind ip and port to sock
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = port_num;
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(listening, (sockaddr *)&server, sizeof(server)) == -1)
    {
        std::cerr << "cannot bind to IP/port" << std::endl;
        close(listening);
        return -4;
    }

    if (listen(listening, SOMAXCONN) == -1)
    {
        std::cerr << "cannot listen" << std::endl;
        close(listening);
        return -5;
    }

    // initialize master fd_set
    fd_set master; // master set of sockets declaration
    FD_ZERO(&master);
    int max_sd = listening; // max socket descriptor
    FD_SET(listening, &master); // add listening socket to master set of sockets

    // timeval struct initialized to 3 minutes, if no activity performed after 3 minutes
    // program will end, set as last parameter of select()
    // struct timeval timeout;
    // timeout.tv_sec = 3 * 60;
    // timeout.tv_usec = 0;

    while (run_server)
    {
        fd_set working_set = master; // copy bc calling select() is destructive
        // select returns number of fds in three returned descriptor sets
        int socket_count = select(max_sd + 1, &working_set, nullptr, nullptr, nullptr); // with timeval (last param.) == null, select blocks waiting for a fd to become ready
        if (socket_count == -1)
        {
            std::cerr << "select() error" << std::endl;
            break;
        }
        if (socket_count == 0)
        {
            std::cerr << "timeout occured on select()" << std::endl;
            break;
        }

        // counting ready descriptors
        int desc_ready = socket_count;
        for (int i = 0; i <= max_sd && desc_ready > 0; i++)
        {
            // auto tmp_socket = working_set.fd_array[i]; // for windows impl, unix has not got fd_array
            // check if this descriptor is ready
            if (FD_ISSET(i, &working_set))
            {
                desc_ready -= 1; // in order to stop loop when all ready descriptors are found
                // check if descriptor refers to listening socket
                if (i == listening)
                {
                    std::cout << "listening socket is readable" << std::endl;
                    // accept all incoming connection, if accept() fails with EWOULDBLOCK
                    int new_cli = 0;
                    while (new_cli != -1)
                    {
                        // if accept() fails with EWOULDBLOCK, then all connections have been accepted
                        // otherwise accept failure leads to server's shutdown
                        sockaddr_in client;
                        socklen_t client_size = sizeof(client);
                        new_cli = accept(listening, (sockaddr *)&client, &client_size);
                        //new_cli = accept(listening, nullptr, nullptr);
                        if (new_cli == -1)
                        {
                            if (errno != EWOULDBLOCK)
                            {
                                // other error
                                std::cerr << "accept() error" << std::endl;
                                run_server = false;
                            }
                            // EWOULDBLOCK signal
                            break;
                        }
                        std::cout << "new incoming connection descriptor: " << new_cli << std::endl;
                        // add incoming connection to master set
                        FD_SET(new_cli, &master);
                        print_host(client);
                        if (new_cli > max_sd)
                            max_sd = new_cli;
                        // loop back and accept another incoming connection
                    } // while new_cli != -1
                } // if i == listening

                // not a listening socket, but readable
            } // if ISSET
            // else
            // {
            //     // accept new mess
            // }
        }
    }
    // cleanup of open sockets
    for (int socket = 0; i <= max_sd; i++)
    {
        if (FD_ISSET(socket, &master))
            close(socket);
    }
    return 0;
}
