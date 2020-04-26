#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
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
    bool close_conn = false;
    int buff_size = 4096;
    char buffer[buff_size];
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
        for (int descriptor = 0; descriptor <= max_sd && desc_ready > 0; descriptor++)
        {
            // auto tmp_socket = working_set.fd_array[descriptor]; // for windows impl, unix has not got fd_array
            // check if this descriptor is ready
            if (FD_ISSET(descriptor, &working_set))
            {
                desc_ready -= 1; // in order to stop loop when all ready descriptors are found
                // check if descriptor refers to listening socket
                if (descriptor == listening)
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
                        if (new_cli < 0)
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
                        std::string welcome_msg = "Hello from server!\n";
                        send(new_cli, welcome_msg.c_str(), welcome_msg.size() + 1, 0);
                        print_host(client);
                        if (new_cli > max_sd)
                            max_sd = new_cli;
                        // loop back and accept another incoming connection
                    } // while new_cli != -1
                } // if descriptor == listening

                // not a listening socket, but readable
                else
                {
                    std::cout << "descriptor " << descriptor << " is readable" << std::endl;
                    close_conn = false;
                    // receive all incoming data on this socket before looping back and calling select()
                    while (true)
                    {
                        memset(buffer, 0, buff_size);
                        // check if recv blocks
                        int bytes_recv = recv(descriptor, buffer, buff_size, 0);
                        if (bytes_recv < 0)
                        {
                            if (errno != EWOULDBLOCK)
                            {
                                std::cerr << "recv() error" << std::endl;
                                close_conn = true;
                            }
                            break;
                        }
                        // check if the connection closed by client
                        if (bytes_recv == 0)
                        {
                            std::cout << "client disconnected" << std::endl;
                            close_conn = true;
                            break;
                        }

                        //echo message back
                        std::cout << "received: " << std::string(buffer, 0, bytes_recv);
                        std::string echo_msg = "echo from server: ";
                        echo_msg.append(std::string(buffer, 0, bytes_recv));
                        if (send(descriptor, echo_msg.c_str(), echo_msg.size() + 1, 0) < 0)
                        {
                            std::cerr << "send() error" << std::endl;
                            close_conn = true;
                            break;
                        }

                        // send other clients the message
                        // for (int out_socket = 0; out_socket < master.fd_count; out_socket++)

                    } // while
                    // if the close_conn was set true, we need to clean up and change number of descriptors
                    if (close_conn)
                    {
                        close(descriptor);
                        FD_CLR(descriptor, &master);
                        if (descriptor == max_sd)
                        {
                            // look for new max_sd value
                            while (FD_ISSET(max_sd, &master) == false)
                                max_sd -= 1; // decrement max_sd value till found new max descriptor value
                        }
                    }
                } // end of readable descriptor
            } // end of if ISSET
        } // end of loop through selectable descriptors
    }
    // cleanup of open sockets
    for (int socket = 0; socket <= max_sd; socket++)
    {
        if (FD_ISSET(socket, &master))
            close(socket);
    }
    return 0;
}
