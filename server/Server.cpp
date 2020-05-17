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
    while (run_server)
    {
        fd_set read_set = master;
        fd_set write_set;
        int socket_count;
        if (conn_to_write > 0)
        {
            write_set = master;
            socket_count = select_fds(max_sd + 1, &read_set, &write_set);
        }
        else
            socket_count = select_fds(max_sd + 1, &read_set);
        if (socket_count < 1)
            break;
        int desc_ready = socket_count;
        for (int current_desc = 0; current_desc <= max_sd && desc_ready > 0; current_desc++)
        {
            std::cout << "current desc: " << current_desc << " socket count: " << socket_count << "ready descs: " << desc_ready << std::endl;
            if (FD_ISSET(current_desc, &read_set))
            {
                desc_ready -= 1;
                if (current_desc == listening)
                    run_server = handle_new_connection(listening);
                else if (!is_logged(current_desc) || (!is_logged(current_desc) && get_socket_write_state(current_desc) != IDLE)) /*TODO*/
                    handle_existing_incoming_connection(current_desc);
            }
            // handle situation when server must be the one to initialize communication
            if (conn_to_write > 0 && FD_ISSET(current_desc, &write_set))
            {
                std::cout << "in writing sockets" << std::endl;
                desc_ready -= 1;
                std::cout << "desc ready: " << desc_ready << std::endl;
                if (desc_to_login.count(current_desc))
                {
                    int login_info = client_login(current_desc);
                    if (login_info == MSG_SENT)
                    	std::cout << "header sent" << std::endl;
//                    else if (login_info == 9)
//                    	conn_to_write -= 1;
                    else if (login_info == WORK_END)
                    	std::cout << "work end occured" << std::endl;
                    else	
                    	std::cout << "else in writing" << std::endl;	
                }
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

    std::cout << std::endl << "blocked on select()";
    if (write_set != nullptr)
    	std::cout << " with write";
    std::cout << std::endl;	
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
        desc_to_login.insert(std::make_pair(new_cli, std::make_pair(0, header_size)));
        init_socket_state(new_cli);
        conn_to_write += 1;
        if (make_nonblocking(new_cli) < 0)
        {
            std::cerr << "make nonblocking error" << std::endl;
            break;
            // res = false; ??
        }
        if (set_socket_opt(new_cli, IPPROTO_TCP, TCP_QUICKACK, true) < 0)
        {
            std::cerr << "make quickack error" << std::endl;
            break;
            // res = false; ??
        }
        // connection established

        //std::string welcome_msg = "Hello from server!\n";
        //send(new_cli, welcome_msg.c_str(), welcome_msg.size() + 1, 0);
        //client_login(sockfd);

        print_host(client);
        if (new_cli > max_sd)
            max_sd = new_cli;
        // loop back and accept another incoming connection
    } // while
    return res;
}

void Server::handle_existing_incoming_connection(int sockfd)
{
    std::cout << "descriptor " << sockfd << " is readable" << std::endl;
    //int buff_size = 4096;
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
        //else
        //{
            //if login incomplete
            //if (desc_to_login.count(sockfd))
            //{
            //    if(client_login(sockfd) < 0)
            //        change_cli = true;
            //}
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
        //}
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

int Server::client_login(int sockfd)
{
    int res;
    int req_len = get_byte_width(LOGIN_REQ);
    // if first header
    std::cout << "in login func sockfd: " << sockfd << ", to SEND: " << desc_to_login[sockfd].second << " to RECV: " << desc_to_login[sockfd].first << " and req_len: " << req_len << std::endl;
    if (desc_to_login[sockfd].first == 0 && desc_to_login[sockfd].second == header_size)
    {
        res = send_header(STATEMENT, req_len, sockfd);
        if (res == -1)
        {
            std::cout << "blocking operation, return to select" << std::endl;
            return -1;
        }
        else if (res == header_size)
        {
            std::cout << "success sent " << res << " bytes, REQ len:" << req_len << std::endl;
            desc_to_login[sockfd].first = 0;
            desc_to_login[sockfd].second = req_len;
            // now send message
            //conn_to_write -= 1;
            //return LOGIN_SUCCESSFUL;
        }
        else if (res > -1 && res < header_size)
        {
        	std::cout << "partial send: " << res << " bytes" << std::endl;
        	desc_to_login[sockfd].first = 0;
        	desc_to_login[sockfd].second = header_size - res;
        	return LOGIN_INCOMPLETE;
        }
        else
        {
        	std::cout << "other error: " << res;
        	return res;
        }	
    }
    if (desc_to_login[sockfd].first == 0 && desc_to_login[sockfd].second == req_len)
    {
        std::cout << "before send statement" << std::endl;
        res = send_statement(sockfd, LOGIN_REQ, req_len);
        if (res == -1)
        {
            std::cout << "blocking operation, return to select" << std::endl;
            return -1;
        }
        else if (res == req_len)
        {
            std::cout << "success sent " << res << " bytes in statement" << std::endl;
            desc_to_login[sockfd].first = header_size;
            desc_to_login[sockfd].second = 0;
            // now send message
            conn_to_write -= 1;
            //return LOGIN_SUCCESSFUL;
            return Server::LOGIN_SUCCESSFUL;
        }
        else if (res > -1 && res < header_size)
        {
            std::cout << "partial send: " << res << " bytes" << std::endl;
            desc_to_login[sockfd].first = 0;
            desc_to_login[sockfd].second = req_len - res;
            return LOGIN_INCOMPLETE;
        }
        else
        {
            std::cout << "other error: " << res;
            return res;
        }
    }
    return Server::WORK_END;
}

std::string Server::make_header(int msg_type, int msg_len)
{
    //int msg_type_len = 2;
    std::string result;
    if (msg_type < 10)
        result = std::to_string(0) + std::to_string(msg_type);
    else
        result = std::to_string(msg_type);
    std::string len = std::to_string(msg_len);
    result += len;
    if(result.size() < header_size)
    {
        int diff = header_size - result.size();
        std::string padding;
        for (int i = 0; i < diff; i++)
            padding += std::to_string(0);
        result.insert(msg_type_len, padding);
    }
    return result;
}

int Server::send_header(int msg_type, int msg_len, int sockfd)
{
    std::string header = make_header(msg_type, msg_len);
    // statement made only for login, for now
    if (msg_type == STATEMENT)
    {
        int to_send = desc_to_login[sockfd].second;
        int bytes_sent = 0;
        int bytes_left = header_size;
        int offset = header_size - to_send;
        //char* msg_to_send;
        //prepare_buff_to_send(header_size, to_send, header, msg_to_send);
        auto msg_to_send = header.substr(offset, to_send).c_str();
        bytes_sent = nonblock_send(sockfd, msg_to_send, to_send);
        std::cout << "msg to send 1 char: " << *msg_to_send << std::endl;
        if (bytes_sent == -1)
            return -1; // change client, EWOULDBLOCK occured
        else if (bytes_sent < -1)
            return -2;
        // some data was sent
        return bytes_sent;
    }
    else
        std::cout << "no statement lol";
    return -3;
}

int Server::nonblock_send(int sockfd, const char *buff, int nbytes)
{
    std::cout << "in nonblock send, buff: " << buff[0] << std::endl;
    int bytes_sent = send(sockfd, buff, nbytes, 0);
    if (bytes_sent < 0)
    {
        if (errno != EWOULDBLOCK)
        {
            std::cerr << "send() error" << std::endl;
            return -2;
        }
        std::cerr << "EWOULDBLOCK occured" << std::endl;
        return -1; // change client, EWOULDBLOCK occured
    }
    // some data was sent
    return bytes_sent;
}

int Server::get_byte_width(int num)
{
    if (num < 0)
        return -1;
    int res = 0;
    std::cout << "in get_b_width, num: " << num << std::endl;
    do
    {
        num /= 10;
        res += 1;
    }
    while (num > 0);
    std::cout << "result from width: " << res << std::endl;
    return res;
}

int Server::send_statement(int sockfd, int info, int nbytes)
{
    std::string statement = std::to_string(info);
    int offset = statement.size() - nbytes;
    auto buff = statement.substr(offset, nbytes).c_str();
    //prepare_buff_to_send(statement.size(), nbytes, statement, buff);
    std::cout << "buff to send 1 char: " << *buff << std::endl;
    int bytes_sent = nonblock_send(sockfd, buff, nbytes);
    if (bytes_sent == -1)
        return -1; // change client, EWOULDBLOCK occured
    else if (bytes_sent < -1)
        return -2;
    return bytes_sent;
}

bool Server::is_logged(int sockfd)
{
    return desc_to_login.count(sockfd) > 0;
}

int Server::get_socket_read_state(int sockfd)
{
    return clients_state[sockfd].first.first;
}

int Server::get_socket_read_bytes_number(int sockfd)
{
    return clients_state[sockfd].first.second;
}

int Server::get_socket_write_state(int sockfd)
{
    return clients_state[sockfd].second.first;
}

int Server::get_socket_write_bytes_number(int sockfd)
{
    return clients_state[sockfd].second.second;
}

void Server::set_socket_read_state(int sockfd, int val)
{
    clients_state[sockfd].first.first = val;
}

void Server::set_socket_read_bytes_number(int sockfd, int val)
{
    clients_state[sockfd].first.second = val;
}

void Server::set_socket_write_state(int sockfd, int val)
{
    clients_state[sockfd].second.first = val;
}

void Server::set_socket_write_bytes_number(int sockfd, int val)
{
    clients_state[sockfd].second.second = val;
}

void Server::init_socket_state(int sockfd)
{
    clients_state.insert(std::make_pair(sockfd, std::make_pair(std::make_pair(IDLE, 0), std::make_pair(HEADER_TO_SEND, header_size))));
}