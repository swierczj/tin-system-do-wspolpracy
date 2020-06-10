#ifndef TIN_NETWORKING_H
#define TIN_NETWORKING_H

#include <list>
#include "ClientsMonitor.h"
#include "Message.h"

class Networking
{
private:
    struct NetState
    {
        int msg_type;
        int msg_len;
        NetState() : msg_type(Message::HEADER), msg_len(0) {}
        NetState(int msg_t, int msg_l) : msg_type(msg_t), msg_len(msg_l) {}
    };
    std::map <int, NetState> netstate;
    std::map <int, std::list<NetState>> send_queues;
public:
    bool is_waiting_for_header(int sockfd, ClientsMonitor &cm);
    bool is_waiting_for_msg(int sockfd, ClientsMonitor &cm);
    bool to_send_header(int sockfd, ClientsMonitor &cm);
    bool to_send_msg(int sockfd, ClientsMonitor &cm);
    void add_client(int sockfd);
    void set_msg_type(int sockfd, int msg_t);
    void set_msg_len(int sockfd, int msg_l);
    int get_msg_type(int sockfd);
    int get_msg_len(int sockfd);
    void set_multicast(std::set<int> const &receivers, ClientsMonitor &cm, int msg_t, int msg_len); // possibly add flag immediate, to make service without delay for example when client fetched changes and edited document
    void add_to_send(int sockfd, int msg_type, int msg_len);
    int get_queue_len(int sockfd);
    int get_first_obs_len(int sockfd, int msg_t);
    void remove_sent(int sockfd, int msg_t);

};


#endif //TIN_NETWORKING_H
