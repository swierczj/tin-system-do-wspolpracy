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
//#include <algorithm>
#include <vector>
#include "serv_func.h"
#include "Header.h"
#include "ClientsMonitor.h"
#include "RecvBuffers.h"

Header parse_from_string(std::string const &str);

class Server
{
private:
    typedef int bytes_to_receive;
    typedef int bytes_to_send;
    typedef int msg_state;
    //typedef std::pair<std::pair<msg_state, bytes_to_receive>, std::pair<msg_state, bytes_to_send>> socket_state;
    typedef std::pair<bytes_to_receive, bytes_to_send> bytes_pair;
    typedef char* recv_buffer;

    uint16_t port_number;
    enum server_consts { IDLE = -1, HEADER = -1, HEADER_TO_SEND = 0, HEADER_SENT = 1, MSG_SENT = 2, HEADER_TO_RECV = 0
                       , HEADER_RECVD = 1, MSG_RECVD = 2, LOGIN_SUCCESSFUL = 0, LOGIN_INCOMPLETE = 1, msg_type_len = 2
                       , header_size = 6, default_port = 54000 };
    enum header_msg_type {LOGIN = 0, STATEMENT = 1, EDIT = 2, PUBLIC_KEY = 3};
    enum statement_info {KEEP_ALIVE = 0, LOGIN_REQ = 1, LOGIN_ACC = 2, LOGIN_REJ = 3, LOG_OUT = 4, WORK_END = 5, PUB_KEY_REQ = 6};
    int max_sd;
    int listening;
    fd_set master;
    //std::map<int, socket_state> clients_state;
    std::map<int, bytes_pair> desc_to_login;
    //std::map<int, std::vector<char>> recv_buffers;
    RecvBuffers recv_buffers;
    ClientsMonitor clients;

    
    int set_listening();
    int set_socket(int sock_domain = AF_INET, int sock_type = SOCK_STREAM, int proto = 0);
    int set_socket_opt(int sockfd, int opt_level, int opt_name, bool opt_on);
    int make_nonblocking(int sockfd);
    int bind_address(int sockfd);
    int set_to_listen(int sockfd, int queue_len = SOMAXCONN);
    fd_set initialize_fd_set(int sockfd);
    int select_fds(int nfds, fd_set* read_set = nullptr, fd_set* write_set = nullptr, fd_set* exc_set = nullptr, int timeout_sec = -1);
    //bool is_in_set(int sockfd, fd_set* sock_set);
    bool handle_new_connection(int accept_sd);
    void handle_existing_incoming_connection(int sockfd);
    bool forward_message(int sockfd, char* bytes);
    void remove_socket(int sockfd, int max_desc, fd_set* set);
    void cleanup(int max_desc, fd_set* set); /*TODO: update clients and recv_buffers*/

    int client_login(int sockfd);
    std::string make_header(int msg_type, int msg_len);
    int send_header(int msg_type, int msg_len, int sockfd);
    int nonblock_send(int sockfd, const char* buff, int nbytes);
    int send_statement(int sockfd, int info, int nbytes);
    int get_byte_width(int num);
    void handle_existing_outbound_connection(int sockfd);
    int nonblock_recv(int sockfd, char* buff, int nbytes);
    int receive_message(int sockfd);
    //void cpy_to_recv_buff(char* buff, int len, int sockfd);
    void parse_login_info_from_string(std::string const &msg); /* TODO: class handling password */

    fd_set init_set_with_fds(std::vector<int> const &fds);
public:
    Server(int port = default_port) : port_number(htons(port)), max_sd(-1), listening(-1) {}
    void run();
};

#endif //TIN_SERVER_H