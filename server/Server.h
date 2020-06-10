#ifndef TIN_SERVER_H
#define TIN_SERVER_H

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <sys/ioctl.h>
#include <sstream>
#include <netinet/tcp.h>
#include <map>
#include <vector>
#include "serv_func.h"
#include "Header.h"
#include "Statement.h"
#include "ClientsMonitor.h"
#include "RecvBuffers.h"
#include "LoginHandler.h"
#include "Networking.h"
#include "NotepadHandler.h"
#include "ClientID.h"
#include "Edit.h"

Header parse_from_string(std::string const &str);

class Server
{
private:
//    typedef int bytes_to_receive;
//    typedef int bytes_to_send;

    uint16_t port_number;
    enum server_consts { IDLE = -1, HEADER_TO_SEND = 0, HEADER_SENT = 1, MSG_SENT = 2, HEADER_TO_RECV = 0
                       , HEADER_RECVD = 1, MSG_RECVD = 2, LOGIN_SUCCESSFUL = 0, LOGIN_INCOMPLETE = 1, msg_type_len = 2
                       , header_size = 6, default_port = 54000 };
    int max_sd;
    int listening;
    fd_set master;
    RecvBuffers recv_buffers;
    ClientsMonitor clients;
    LoginHandler login_handler;
    Networking networking;
    NotepadHandler notepad;
    
    int set_listening();
    int set_socket(int sock_domain = AF_INET, int sock_type = SOCK_STREAM, int proto = 0);
    int set_socket_opt(int sockfd, int opt_level, int opt_name, bool opt_on);
    int make_nonblocking(int sockfd);
    int bind_address(int sockfd);
    int set_to_listen(int sockfd, int queue_len = SOMAXCONN);
    fd_set initialize_fd_set(int sockfd);
    int select_fds(int nfds, fd_set* read_set = nullptr, fd_set* write_set = nullptr, fd_set* exc_set = nullptr, int timeout_sec = -1);
    bool handle_new_connection(int accept_sd);
    void handle_existing_incoming_connection(int sockfd);
    void remove_socket(int sockfd, int max_desc, fd_set* set);
    void cleanup(int max_desc, fd_set* set);

    int client_login(int sockfd);
    int nonblock_send(int sockfd, const char* buff, int nbytes);
    int send_nbytes(int sockfd, Message const &msg, int nbytes);
    void handle_existing_outbound_connection(int sockfd);
    int nonblock_recv(int sockfd, char* buff, int nbytes);
    int recv_nbytes(int sockfd, int to_recv);

    fd_set init_set_with_fds(std::vector<int> const &fds);
    int send_msg(int sockfd, ClientsMonitor& cm, Message const &msg);
    int recv_msg(int sockfd, ClientsMonitor &cm, int msg_type);

    //bool login_ended; // for tests
public:
    Server(int port = default_port) : port_number(htons(port)), max_sd(-1), listening(-1)/*, login_ended(false)*/ {}
    void run();
};

#endif //TIN_SERVER_H
