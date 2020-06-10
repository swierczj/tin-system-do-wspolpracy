#include "Server.h"
#include "ClientID.h"
#include "Edit.h"

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
        if (clients.get_to_write_connections_number() > 0)
        {
            auto tmp = clients.get_write_descriptors();
            auto init_list = tmp;
            init_list.erase(init_list.begin(), init_list.end());
            for(auto &elem : tmp)
            {
                if (!clients.login_ended(elem) || (clients.login_ended(elem) && networking.get_queue_len(elem) > 0))
                    init_list.push_back(elem);
            }

            write_set = init_set_with_fds(init_list);
            socket_count = select_fds(max_sd + 1, &read_set, &write_set);
        }
        else
        {
            socket_count = select_fds(max_sd + 1, &read_set);
            FD_ZERO(&write_set);
        }
        if (socket_count < 1)
            break;
        int desc_ready = socket_count;
        for (int current_desc = 0; current_desc <= max_sd && desc_ready > 0; current_desc++)
        {
            std::cout << "current desc: " << current_desc << " socket count: " << socket_count << " ready descs: " << desc_ready << std::endl;
            if (FD_ISSET(current_desc, &read_set))
            {
                std::cout << "check read set" << std::endl;
                desc_ready -= 1;
                if (current_desc == listening)
                    run_server = handle_new_connection(listening);
                else if (!clients.is_logged(current_desc) || clients.get_socket_read_state(current_desc) != IDLE)
                    handle_existing_incoming_connection(current_desc);

            }
            // handle situation when server must be the one to initialize communication
            //std::cout << "socket write state: " << clients.get_socket_write_state(current_desc) << " is set in writing: " << (FD_ISSET(current_desc, &write_set)) << std::endl;
            if ((clients.get_socket_write_state(current_desc) == HEADER_TO_SEND || clients.get_socket_write_state(current_desc) == HEADER_SENT) && FD_ISSET(current_desc, &write_set))
            {
                desc_ready -= 1;
                handle_existing_outbound_connection(current_desc);
            }
            else if (!(clients.get_socket_write_state(current_desc) == HEADER_TO_SEND || clients.get_socket_write_state(current_desc) == HEADER_SENT) && FD_ISSET(current_desc, &write_set))
                desc_ready -= 1;
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
        clients.add_new_cli(new_cli);
        recv_buffers.add_new(new_cli);

        std::cout << "addednew conn, size: " << clients.get_existing_conns() << std::endl;
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
        }
        // connection established

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
    //char buffer[buff_size];
    bool close_conn = false;
    bool change_cli = false;

    while (!close_conn && !change_cli)
    {
        memset(buffer, 0, buff_size);
        // setting socket to be nonblocking
        int res = 0;
        if (!clients.login_ended(sockfd))
        {
            int login_state = client_login(sockfd);
            if (login_state < 0)
            {
                if (login_state != -1)
                {
                    if (login_state == -2)
                        std::cout << "client disconnected" << std::endl;
                    close_conn = true;
                }
                // EWOULDBLOCK or change socket type
                change_cli = true;
            }
            else if (login_state == LOGIN_SUCCESSFUL)
            {
                std::cout << "logged!" << std::endl;
                //desc_to_login.erase(sockfd);
            }
            else
                std::cout << "login in progress" << std::endl;
        }
        // logged user
        else
        {
            if (networking.is_waiting_for_header(sockfd, clients))
            {
                std::cout << "receiving header from logged user" << std::endl;
                res = recv_msg(sockfd, clients, Message::HEADER);
                // EWOULDBLOCK
                if (res < 0)
                {
                    if (res < -1)
                    {
                        if (res == -2)
                        {
                            if (notepad.is_active(sockfd))
                            {
                                notepad.remove_contributor(sockfd);
                                // if last client, save contents to file
                                if (notepad.get_active_users_number() == 0)
                                    notepad.save_to_file();
                            }
                            //std::cout << "closed conn by client" << std::endl << "left to send: " << networking.get_queue_len(sockfd) << std::endl;
                        }
                        close_conn = true;
                    }
                    else
                        change_cli = true;
                }
                else if (res == 1)
                {
                    Header header = parse_from_string(recv_buffers.get_string_from_bytes(sockfd));
                    if (header.get_msg_type() == Message::EDIT && !notepad.is_active(sockfd))
                        notepad.add_client(sockfd);
                    networking.set_msg_type(sockfd, header.get_msg_type());
                    networking.set_msg_len(sockfd, header.get_msg_len());
                    recv_buffers.clear_buff(sockfd);
                    //std::cout << "msg type from header: " << networking.get_msg_type(sockfd) << std::endl << "msg len: " << networking.get_msg_len(sockfd) << std::endl;

                }
            }
            if (networking.is_waiting_for_msg(sockfd, clients))
            {
                auto msg_type = networking.get_msg_type(sockfd);
                res = recv_msg(sockfd, clients, msg_type);
                if (res < 0)
                {
                    if (res < -1)
                        close_conn = true;
                    else
                        change_cli = true;
                }
                else if (res == 1)
                {
                    if (msg_type == Message::EDIT)
                    {
                        auto edit_text = recv_buffers.get_string_from_bytes(sockfd);
                        // "received EDIT msg: " << edit_text << std::endl;
                        notepad.store_changes(sockfd, edit_text);
                        // if client sending EDIT is using system
                        if (!notepad.is_to_fetch_changes(sockfd))
                        {

                            notepad.edit_contents(edit_text);
                            //std::cout << "decoded value of file: "/* << notepad.get_str()*/ << std::endl;
                            // prepare header to send other clients info
                            //networking.set_msg_len(sockfd, edit_text.size());
                            auto multicast_clients = notepad.get_contributors();
                            multicast_clients.erase(sockfd);
                            networking.set_multicast(multicast_clients, clients, Message::EDIT, edit_text.size());
                        }
                        else
                        {
                            networking.set_msg_type(sockfd, Message::EDIT);
                            networking.add_to_send(sockfd, Message::EDIT, -1);
                        }
                        recv_buffers.clear_buff(sockfd);
                    }
                    change_cli = true; // for tests
                }
            }
        }
        //}
    } // while
    // if the close_conn was set true, we need to clean up and change the max descriptor number
    if (close_conn)
    {
        clients.remove_socket(sockfd);
        recv_buffers.delete_buff(sockfd);
        remove_socket(sockfd, max_sd, &master);
    }
}

void Server::handle_existing_outbound_connection(int sockfd)
{
    bool change_client = false;
    int res = 0;
    // send as much data as it's possible otherwise change the client
    while (!change_client)
    {
        std::cout << "handle outbound conn, desc: " << sockfd << std::endl;
        if (!clients.login_ended(sockfd))
        {
            res = client_login(sockfd);
            if (res < 0)
            {
                if (res != -1)
                    std::cerr << "error" << std::endl;
                //EWOULDBLOCK
                //std::cout << "sending sockets, change to receiving" << std::endl;
                change_client = true;
            }
            if (clients.login_ended(sockfd))
                change_client = true;
        }
        else
        {
            std::cout << "logged user may edit the document, some writing from server here" << std::endl;

            if (networking.to_send_header(sockfd, clients))
            {
                int msg_type = networking.get_msg_type(sockfd);
                int msg_len = networking.get_msg_len(sockfd);
                //std::cout << "assigned in sending msg_len: " << msg_len << ", msg type: " << msg_type << std::endl;
                if (msg_type == Message::EDIT)
                {
                    //std::cout << "sending edit msg" << std::endl;
                    if (notepad.is_to_fetch_changes(sockfd))
                    {
                        //std::cout << "to fetch changes header prep" << std::endl;
                        // prepare to send whole file contents
                        msg_len = notepad.get_notepad_current_size();
                        networking.set_msg_len(sockfd, msg_len);
                    }
                    else
                    {
                        // there may be many pending messages, so we are looking for correct one i.e. first occurrence in pending list for given socket
                        msg_len = networking.get_first_obs_len(sockfd, msg_type);
                        networking.set_msg_len(sockfd, msg_len);
                    }
                }
                //std::cout << "before sending header with edit: " << msg_len << " type: " << msg_type << std::endl;
                res = send_msg(sockfd, clients, Header(msg_type, msg_len));
                if (res == -1)
                {
                    std::cout << "blocking operation in send, return to select" << std::endl;
                    change_client = true;
                }
                else if (res < -1)
                {
                    std::cout << "other error: " << res;
                }
            }
            if (networking.to_send_msg(sockfd, clients))
            {
                // get type and len of msg to send
                auto msg_type = networking.get_msg_type(sockfd);
                auto msg_len = networking.get_msg_len(sockfd);
                if (msg_type == Message::EDIT)
                {
                    std::string edit_msg;
                    if (notepad.is_to_fetch_changes(sockfd))
                        edit_msg = notepad.get_file_as_str();
                    else
                        edit_msg = notepad.get_changes(sockfd);
                    res = send_msg(sockfd, clients, Edit(edit_msg));
                    if (res == -1)
                    {
                        std::cout << "blocking operation in send, return to select" << std::endl;
                        change_client = true;
                    }
                    else if (res == 1)
                    {
                        if (notepad.is_to_fetch_changes(sockfd))
                        // message with file sent so data fetched, now apply changes stored
                        {
                            std::string edit_text = notepad.get_stored_changes(sockfd); // may not work properly, someone would have modified the file
                            // edit file
                            notepad.get_stored_changes(sockfd);
                            notepad.edit_contents(edit_text);
                            // prepare header to send other clients info about edits
                            auto multicast_clients = notepad.get_contributors();
                            multicast_clients.erase(sockfd);
                            networking.set_multicast(multicast_clients, clients, Message::EDIT, edit_text.size());
                        }
                        else
                        {
                            notepad.remove_sent_changes(sockfd);
                            networking.remove_sent(sockfd, msg_type);
                        }
                        change_client = true; // now recv some
                    }
                }

            }
        }
    }
}

void Server::remove_socket(int sockfd, int max_desc, fd_set* set)
{
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
    for (int socket = 0; socket <= max_desc; socket++)
    {
        if (FD_ISSET(socket, set))
            close(socket);
    }
}

int Server::client_login(int sockfd)
{
    int res;
    bool switch_client = false;
    //std::cout << "in login func sockfd: " << sockfd << std::endl;

    while (!switch_client)
    {
        std::cout << "in login loop" << std::endl;
        auto [msg_type, msg_len, info] = clients.get_login_state(sockfd);
        // if header or its part has to be sent
        if (clients.get_socket_write_state(sockfd) == HEADER_TO_SEND)
        {

            res = send_msg(sockfd, clients, Header(msg_type, msg_len));
            if (res == -1)
            {
                std::cout << "blocking operation, return to select" << std::endl;
                switch_client = true;
            }
            else if (res < -1)
            {
                std::cout << "other error: " << res;
                return res;
            }
        }

        if (clients.get_socket_write_state(sockfd) == HEADER_SENT && !switch_client)
        {
            //std::cout << "before send message after sending header, to send: " << clients.get_socket_write_bytes_number(sockfd) << std::endl;

            if (msg_type == Message::STATEMENT)
                res = send_msg(sockfd, clients, Statement(info));
            else if (msg_type == Message::CLIENT_ID)
                res = send_msg(sockfd, clients, ClientID(sockfd));
            if (res == -1)
            {
                std::cout << "blocking operation, return to select" << std::endl;
                switch_client = true;
            }
            // msg after header sent
            else if (res == 1)
            {
                if (msg_type == Message::CLIENT_ID)
                {
                    std::cout << "client id sent" << std::endl;
                    clients.update_logged(sockfd);
                }
                switch_client = true; // now recv some

            }
            else
            {
                std::cout << "other error: " << res;
                return res;
            }
        }
        if (clients.get_socket_read_state(sockfd) == HEADER_TO_RECV &&
            clients.get_socket_write_state(sockfd) == MSG_SENT && !clients.login_ended(sockfd) && !switch_client)
        {
            std::cout << "receiving header before login info" << std::endl;
            res = recv_msg(sockfd, clients, Message::HEADER);
            if (res == -1)
            {
                std::cout << "blocking operation, return to select" << std::endl;
                switch_client = true;
            }
            else if (res == 1)
                recv_buffers.clear_buff(sockfd);
            else if (res == -2)
            {
                std::cout << "connection closed" << std::endl;
                return res;
            }
            else
            {
                std::cout << "other error: " << res;
                return res;
            }
        }
        if (clients.get_socket_read_state(sockfd) == HEADER_RECVD && clients.get_socket_write_state(sockfd) == MSG_SENT
        && !clients.login_ended(sockfd))
        {
            std::cout << "header recvd, now get login" << std::endl;
            res = recv_msg(sockfd, clients, Message::LOGIN);
            if (res == -1)
            {
                std::cout << "blocking operation, return to select" << std::endl;
                switch_client = true;
            }
            else if (res == 1)
            {
                /* parse msg */
                int login_result = login_handler.check_login(recv_buffers.get_string_from_bytes(sockfd));
                if (login_result == LoginHandler::CORRECT)
                {
                    if (clients.is_rejected(sockfd))
                        clients.remove_rejected(sockfd);
                    clients.add_logged_client(sockfd);
                    networking.add_client(sockfd);
                }
                else
                    clients.add_rejected_login(sockfd);
                // cleanup buffer
                recv_buffers.clear_buff(sockfd);
                switch_client = true;
            }
            else if (res == -2)
            {
                std::cout << "connection closed" << std::endl;
                return res;
            }
            else
            {
                std::cout << "other error: " << res;
                return res;
            }
        }
    }

    if (clients.login_ended(sockfd))
        return Server::LOGIN_SUCCESSFUL;
    return -1;
}

int Server::nonblock_send(int sockfd, const char *buff, int nbytes)
{
    int bytes_sent = send(sockfd, buff, nbytes, 0);
    if (bytes_sent < 0)
    {
        if (errno != EWOULDBLOCK)
        {
            std::cerr << "send() error" << std::endl;
            return -2;
        }
        std::cerr << "EWOULDBLOCK occurred" << std::endl;
        return -1; // change client, EWOULDBLOCK occured
    }
    // some data was sent
    return bytes_sent;
}

int Server::send_nbytes(int sockfd, Message const &msg, int nbytes)
{
    std::string msg_bytes = msg.get_string_rep();
    int offset = msg_bytes.size() - nbytes;
    auto buff = msg_bytes.substr(offset, nbytes).c_str();
    int bytes_sent = nonblock_send(sockfd, buff, nbytes);
    if (bytes_sent == -1)
        return -1; // change client, EWOULDBLOCK occured
    else if (bytes_sent < -1)
        return -2;
    return bytes_sent;
}

int Server::nonblock_recv(int sockfd, char *buff, int nbytes)
{
    int bytes_recv = recv(sockfd, buff, nbytes, 0);
    if (bytes_recv < 0)
    {
        if (errno != EWOULDBLOCK)
        {
            std::cerr << "recv() error" << std::endl;
            return -2;
        }
        std::cerr << "EWOULDBLOCK occured" << std::endl;
        return -1; // change client, EWOULDBLOCK occured
    }
    // some data was sent or connection lost
    return bytes_recv;
}

int Server::recv_nbytes(int sockfd, int to_recv)
{
    char buff[to_recv];
    int bytes_recv = 0;
    bytes_recv = nonblock_recv(sockfd, buff, to_recv);
    if (bytes_recv == -1)
        return -1; // change client, EWOULDBLOCK occured
    else if (bytes_recv == 0 )
        return -2;
    else if (bytes_recv < -1)
        return -3;

    recv_buffers.cpy_to_recv_buff(buff, bytes_recv, sockfd);
    return bytes_recv;
}

fd_set Server::init_set_with_fds(const std::vector<int> &fds)
{
    //std::cout << "in init with fds, vec size: " << fds.size() << std::endl;
    fd_set sock_set;
    FD_ZERO(&sock_set);
    for (auto sockfd : fds)
        FD_SET(sockfd, &sock_set);
    return sock_set;
}

int Server::send_msg(int sockfd, ClientsMonitor& cm, Message const &msg)
{
    int to_send = cm.get_socket_write_bytes_number(sockfd);
    int res = send_nbytes(sockfd, msg, to_send);
    if (res == -1)
    {
        std::cout << "blocking operation, return to select" << std::endl;
        return -1;
    }
    else if (res == to_send)
    {
        std::cout << "success sent all " << res << " bytes in msg" << std::endl;
        int to_send_bytes = 0;
        int to_send_state = MSG_SENT;
        if (msg.get_message_type() != Message::HEADER)
        {
            cm.set_socket_read_state(sockfd, HEADER_TO_RECV);
            cm.set_socket_read_bytes_number(sockfd, header_size);
        }
        else
        {
            std::string msg_len_str = msg.get_string_rep().substr(Header::msg_type_len, Header::header_size - Header::msg_type_len);
            to_send_bytes = std::stoi(msg_len_str);
            to_send_state = HEADER_SENT;
        }
        cm.set_socket_write_state(sockfd, to_send_state);
        cm.set_socket_write_bytes_number(sockfd, to_send_bytes);
        return 1;
    }
    else if (res > 0 && res < to_send)
    {
        std::cout << "partial send: " << res << " bytes" << std::endl;
        cm.set_socket_write_bytes_number(sockfd, to_send - res);
        return 0;
    }
    else
    {
        std::cout << "other error: " << res;
        return res;
    }
}

int Server::recv_msg(int sockfd, ClientsMonitor &cm, int msg_type)
{
    int to_recv = cm.get_socket_read_bytes_number(sockfd);
    //std::cout << "in recv_msg to recv: " << to_recv << std::endl;
    int res = recv_nbytes(sockfd, to_recv);
    if (res == -1)
    {
        std::cout << "blocking operation, return to select" << std::endl;
        return -1;
    }
    else if (res == to_recv)
    {
        std::cout << "success recvd all " << res << " bytes" << std::endl;
        int to_recv_bytes = 0;
        int to_recv_state = MSG_RECVD;
        if (msg_type != Message::HEADER)
        {
            cm.set_socket_write_state(sockfd, HEADER_TO_SEND);
            cm.set_socket_write_bytes_number(sockfd, header_size);
        }
        else
        {
            Header header = parse_from_string(recv_buffers.get_string_from_bytes(sockfd));
            to_recv_bytes = header.get_msg_len();
            to_recv_state = HEADER_RECVD;
        }

        cm.set_socket_read_state(sockfd, to_recv_state);
        cm.set_socket_read_bytes_number(sockfd, to_recv_bytes);
        // cleanup the buffer
        return 1;
    }
    else if (res > 0 && res < to_recv)
    {
        std::cout << "partial recv: " << res << " bytes" << std::endl;
        std::string recvd = recv_buffers.get_string_from_bytes(sockfd);
//        for (auto c : recvd)
//            std::cout << c << std::endl;
        //std::cout << "recvd: " << recv_buffers.get_string_from_bytes(sockfd) << std::endl;
        cm.set_socket_read_bytes_number(sockfd, to_recv - res);
        return 0;
    }
    else if (res == -2)
    {
        std::cout << "connection closed" << std::endl;
        return -2;
    }
    else
    {
        std::cout << "other error: " << res;
        return res;
    }
}

