#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <sys/ioctl.h>

//windows
#include <winsock.h>
#include <windows.h>
#include <ws2tcpip.h>


int main()
{
    auto port_num = htons(50000);
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        std::cerr << "cannot create a socket" << std::endl;
        return -1;
    }

    // set listening socket to be nonblocking
    int opt_arg = 1;
    char* argp = (char*)&opt_arg;
    if(ioctl(listening, FIONBIO, argp) == -1)
    {
        std::cerr << "ioctl() error" << std::endl;
        close(listening);
        return -2;
    }
    /*TODO*/
    //bind ip and port to sock
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = port_num;
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(listening, (sockaddr *) &server, sizeof(server)) == -1)
    {
        std::cerr << "cannot bind to IP/port" << std::endl;
        close(listening);
        return -3;
    }

    if (listen(listening, SOMAXCONN) == -1)
    {
        std::cerr << "cannot listen" << std::endl;
        close(listening);
        return -4;
    }

    // wait for connection
    fd_set master; // master set of sockets declaration
    FD_ZERO(&master);
    int max_sd = listening; // max socket descriptor
    FD_SET(listening, &master); // add listening socket to master set of sockets

    // timeval struct initialised to 3 minutes, if no activity performed after 3 minutes
    // program will end, set as last parameter of select()
    // struct timeval timeout;
    // timeout.tv_sec = 3 * 60;
    // timeout.tv_usec = 0;

    while (true)
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

        int desc_ready = socket_count;
        for (int i = 0; i < max_sd && desc_ready > 0; i++)
        {
            // auto tmp_socket = working_set.fd_array[i]; // for windows impl, unix has not got fd_array
            // check if this descriptor is ready
            if (FD_ISSET(i, &working_set))
            {
                desc_ready -= 1; // in order to stop loop when all ready descriptors are found
                // check if desc refer listening socket
                if (i == listening)
                {
                    // accept each incoming connection, if accept() fails with EWOULDBLOCK
                }
                int new_cli = accept(listening, nullptr, nullptr);
            }
            // else
            // {
            //     // accept new mess
            // }
        }
    }
    close(client_socket);
    return 0;
}
