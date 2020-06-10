//#include <sys/time.h>
//#include <sys/types.h>
//#include <cstring>
//#include <string>
//#include <iostream>
//#include <sstream>
#include "Server.h"
//#include "serv_func.h"

//#include

void Server::run()
{
    listening = set_listening();
    if (listening < 0)
        return;
    master = initialize_fd_set(listening);
    max_sd = listening;
    bool run_server = true;
    while(run_server)
    {
        fd_set working_set = master;
        int socket_count = select_fds(max_sd + 1, &working_set);
        if (socket_count < 1)
            break;
        int desc_ready = socket_count;
        for (int current_desc = 0; current_desc <= max_sd && desc_ready > 0; current_desc++)
        {
            if (FD_ISSET(current_desc, &working_set))
            {
                desc_ready -= 1;
                if (current_desc == listening)
                    run_server = handle_new_connection(listening);
                else
                    handle_existing_connection(current_desc);
            }
        }
    }
    cleanup(max_sd, &master);
}

int Server::set_listening()
{
    int sockfd = set_socket();
    if (sockfd < 0)
        return -1;
    // set listening socket to be reusable
    if (set_socket_opt(sockfd, SOL_SOCKET, SO_REUSEADDR, true) < 0)
    {
        close(sockfd);
        return -1;
    }
    if (make_nonblocking(sockfd) < 0)
    {
        close(sockfd);
        return -1;
    }
    if (bind_address(sockfd) < 0)
    {
        close(sockfd);
        return -1;
    }
    if (set_to_listen(sockfd) < 0)
    {
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int Server::set_socket(int sock_domain, int sock_type, int proto)
{
    int sockfd = socket(sock_domain, sock_type, proto);
    if (sockfd == -1)
        std::cerr << "cannot create a socket" << std::endl;
    return sockfd;
}

int Server::set_socket_opt(int sockfd, int opt_level, int opt_name, bool opt_on)
{
    int opt_val;
    if (opt_on)
        opt_val = 1;
    else
        opt_val = 0;
    int res = setsockopt(sockfd, opt_level, opt_name, &opt_val, sizeof(opt_val));
    if (res == -1)
        std::cerr << "setsockopt() error" << std::endl;
        //close(listening);
    return res;
}

int Server::make_nonblocking(int sockfd)
{
    int on = 1;
    //void* argp = &on;
    int res = ioctl(sockfd, FIONBIO, &on);
    if(res == -1)
        std::cerr << "ioctl() error" << std::endl;
    return res;
}

int Server::bind_address(int sockfd)
{
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = port_number;
    server.sin_addr.s_addr = INADDR_ANY;

    int res = bind(sockfd, (sockaddr *)&server, sizeof(server)) == -1;
    if (res == -1)
        std::cerr << "cannot bind to IP/port" << std::endl;
    return res;
}

int Server::set_to_listen(int sockfd, int queue_len)
{
    int res = listen(sockfd, queue_len);
    if (res == -1)
        std::cerr << "cannot listen" << std::endl;
    return res;
}

fd_set Server::initialize_fd_set(int sockfd)
{
    fd_set sock_set;
    FD_ZERO(&sock_set);
    FD_SET(sockfd, &sock_set);
    return sock_set;
}
// timeout argument represents desired timout value in milliseconds
int Server::select_fds(int nfds, fd_set* read_set, fd_set* write_set, fd_set* exc_set, int timeout_sec)
{
    int res;
    struct timeval* timeout_ptr;
    if (timeout_sec < 0)
        timeout_ptr = nullptr;
    else
    {
        struct timeval timeout;
        timeout.tv_sec = timeout_sec;
        timeout.tv_usec = 0;
        timeout_ptr = &timeout;
    }

    res = select(nfds + 1, read_set, write_set, exc_set, timeout_ptr);
    if (res == -1)
        std::cerr << "select() error" << std::endl;
    if (res == 0)
        std::cerr << "timeout occured on select()" << std::endl;
    return res;
}

bool Server::handle_new_connection(int accept_sd)
{
    int new_cli = 0;
    int res = true;
    while (new_cli != -1)
    {
        // if accept() fails with EWOULDBLOCK, then all connections have been accepted
        // otherwise accept failure leads to server's shutdown
        sockaddr_in client;
        socklen_t client_size = sizeof(client);
        new_cli = accept(accept_sd, (sockaddr *)&client, &client_size);
        if (new_cli < 0)
        {
            if (errno != EWOULDBLOCK)
            {
                // other error
                std::cerr << "accept() error" << std::endl;
                res = false;
            }
            // EWOULDBLOCK signal
            break;
        }
        std::cout << "new incoming connection descriptor: " << new_cli << std::endl;
        // add incoming connection to master set
        FD_SET(new_cli, &master);
        // login below?
        /* TODO */
        std::string welcome_msg = "Hello from server!\n";
        send(new_cli, welcome_msg.c_str(), welcome_msg.size() + 1, 0);
        print_host(client);
        if (new_cli > max_sd)
            max_sd = new_cli;
        // loop back and accept another incoming connection
    } // while
    return res;
}

void Server::handle_existing_connection(int sockfd)
{
    std::cout << "descriptor " << sockfd << " is readable" << std::endl;
    int buff_size = 4096;
    char buffer[buff_size];
    bool close_conn = false;
    bool change_cli = false;
    // receive all incoming data on this socket before looping back and calling select()
    while (!close_conn && !change_cli)
    {
        memset(buffer, 0, buff_size);
        // setting socket to be nonblocking
        if (make_nonblocking(sockfd) < 0)
            close_conn = true;
        else
        {
            std::cout << "blocked on recv()" << std::endl;
            int bytes_recv = recv(sockfd, buffer, buff_size, 0);
            if (bytes_recv < 0)
            {
                if (errno != EWOULDBLOCK)
                {
                    std::cerr << "recv() error" << std::endl;
                    close_conn = true;
                }
                change_cli = true;
                //break;
                
            }
            else if (bytes_recv == 0)
            {
                std::cout << "client disconnected" << std::endl;
                close_conn = true;
            }
            else
            {
                if (set_socket_opt(sockfd, IPPROTO_TCP, TCP_QUICKACK, true) < 0)
                    close_conn = true;
                else
                {
                    std::cout << "server received: " << std::string(buffer, 0, bytes_recv) << std::endl;
                    close_conn = forward_message(sockfd, buffer);
                }
            }
        }
    } // while
    // if the close_conn was set true, we need to clean up and change the max descriptor number
    if (close_conn)
        remove_socket(sockfd, max_sd, &master);
}

bool Server::forward_message(int sockfd, char* bytes)
{
    bool res = false;
    for (int tmp_desc = 0; tmp_desc <= max_sd; tmp_desc++)
    {
        if (FD_ISSET(tmp_desc, &master) && tmp_desc != listening && tmp_desc != sockfd)
        {
            std::ostringstream ss;
            ss << "SOCKET#" << tmp_desc << ": " << bytes;
            std::string msg = ss.str();
            if (send(tmp_desc, msg.c_str(), msg.size() + 1, 0) < 0)
            {
                std::cerr << "send() error from SOCK#" << tmp_desc << std::endl;
                res = true;
                break;
            }
        }
    }
    return res;
}

void Server::remove_socket(int sockfd, int max_desc, fd_set* set)
{
    //std::cout << "removing socket" << std::endl;
    close(sockfd);
    FD_CLR(sockfd, set);
    if (sockfd == max_desc)
    {
        // look for new max_desc value
        while (FD_ISSET(max_desc, &master) == false)
            max_desc -= 1; // decrement max_desc value till found new max descriptor value
    }
}

void Server::cleanup(int max_desc, fd_set* set)
{
    //std::cout << "cleaning up" << std::endl;
    for (int socket = 0; socket <= max_desc; socket++)
    {
        if (FD_ISSET(socket, set))
            close(socket);
    }
}
