#ifndef TIN_CLIENTSMONITOR_H
#define TIN_CLIENTSMONITOR_H

#include <utility>
#include <map>
#include <set>
#include "Header.h"

class ClientsMonitor
{
public:
    typedef int bytes_to_receive;
    typedef int bytes_to_send;
    typedef int msg_state;
    typedef std::pair<std::pair<msg_state, bytes_to_receive>, std::pair<msg_state, bytes_to_send>> socket_state;
    enum socket_read_states {IDLE = -1, HEADER_TO_RECV = 0, HEADER_RECVD = 1, MSG_RECVD = 2};
    enum socket_write_states {HEADER_TO_SEND = 0, HEADER_SENT = 1, MSG_SENT = 2};

private:
    std::map <int, socket_state> clients_state;
    std::set<int> logged_clients;
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

};


#endif //TIN_CLIENTSMONITOR_H
