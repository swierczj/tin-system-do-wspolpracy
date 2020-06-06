#ifndef TIN_CLIENTSMONITOR_H
#define TIN_CLIENTSMONITOR_H

#include <utility>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include "Header.h"
#include "Statement.h"
#include "serv_func.h"

class ClientsMonitor
{
public:
    typedef int bytes_to_receive;
    typedef int bytes_to_send;
    typedef int msg_state;
    typedef bool accept_sent;
    typedef std::pair<std::pair<msg_state, bytes_to_receive>, std::pair<msg_state, bytes_to_send>> socket_state;
    enum socket_read_states {IDLE = -1, HEADER_TO_RECV = 0, HEADER_RECVD = 1, MSG_RECVD = 2};
    enum socket_write_states {HEADER_TO_SEND = 0, HEADER_SENT = 1, MSG_SENT = 2};
    struct LoginState
    {
        int msg_type;
        int msg_len;
        int info;
    };
private:
    std::map <int, socket_state> clients_state;
    std::map <int, accept_sent> logged_clients;
    std::set <int> rejected_logins;
    //std::map <int, int>
public:
    void add_new_cli(int sockfd);
    int get_socket_read_state(int sockfd);
    int get_socket_read_bytes_number(int sockfd);
    int get_socket_write_state(int sockfd);
    int get_socket_write_bytes_number(int sockfd);
    void set_socket_read_state(int sockfd, int val);
    void set_socket_read_bytes_number(int sockfd, int val);
    void set_socket_write_state(int sockfd, int val);
    void set_socket_write_bytes_number(int sockfd, int val);
    int get_to_write_connections_number();
    bool is_logged(int sockfd);
    int get_existing_conns();
    std::vector<int> get_write_descriptors();
    void add_logged_client(int sockfd);
    void add_rejected_login(int sockfd);
    void remove_rejected(int sockfd);
    bool is_rejected(int sockfd);
    void update_logged(int sockfd);
    bool login_ended(int sockfd);
    LoginState get_login_state(int sockfd);
    void remove_socket(int sockfd);
};


#endif //TIN_CLIENTSMONITOR_H
