#include "Networking.h"

bool Networking::is_waiting_for_header(int sockfd, ClientsMonitor &cm)
{
    return cm.get_socket_read_state(sockfd) == ClientsMonitor::HEADER_TO_RECV || cm.get_socket_read_state(sockfd) == ClientsMonitor::MSG_SENT;
}

bool Networking::is_waiting_for_msg(int sockfd, ClientsMonitor &cm)
{
    std::cout << "in is waiting for msg func state: " << cm.get_socket_read_state(sockfd) << std::endl;
    return cm.get_socket_read_state(sockfd) == ClientsMonitor::HEADER_RECVD;
}

void Networking::add_client(int sockfd)
{
    netstate.insert(std::make_pair(sockfd, NetState()));
}

void Networking::set_msg_type(int sockfd, int msg_t)
{
    auto it = netstate.find(sockfd);
    if (it != netstate.end())
        it->second.msg_type = msg_t;
}

void Networking::set_msg_len(int sockfd, int msg_l)
{
    auto it = netstate.find(sockfd);
    if (it != netstate.end())
        it->second.msg_len = msg_l;
}

int Networking::get_msg_type(int sockfd)
{
    auto it = netstate.find(sockfd);
    if (it != netstate.end())
        return it->second.msg_type;
    return -2;
}

int Networking::get_msg_len(int sockfd)
{
    auto it = netstate.find(sockfd);
    if (it != netstate.end())
        return it->second.msg_len;
    return -1;
}

bool Networking::to_send_header(int sockfd, ClientsMonitor &cm)
{
    return cm.get_socket_write_state(sockfd) == ClientsMonitor::HEADER_TO_SEND;
}

bool Networking::to_send_msg(int sockfd, ClientsMonitor &cm)
{
    return cm.get_socket_write_state(sockfd) == ClientsMonitor::HEADER_SENT;
}

void Networking::set_multicast(const std::set<int> &receivers, ClientsMonitor &cm, int msg_t, int msg_len)
{
    for (auto const &elem : receivers)
    {
        cm.set_socket_write_bytes_number(elem, msg_len);
        cm.set_socket_write_state(elem, ClientsMonitor::HEADER_TO_SEND);
        set_msg_type(elem, msg_t);
        set_msg_len(elem, msg_len);
        add_to_send(elem, msg_t, msg_len);
    }
}

void Networking::add_to_send(int sockfd, int msg_type, int msg_len)
{
    send_queues[sockfd].emplace_back(NetState(msg_type, msg_len));
}

int Networking::get_queue_len(int sockfd)
{
    return send_queues[sockfd].size();
}

int Networking::get_first_obs_len(int sockfd, int msg_t)
{
    bool found;
    int res = -1;
    for (auto i = send_queues[sockfd].begin(); i != send_queues[sockfd].end() && !found; ++i)
    {
        if (i->msg_type == msg_t)
        {
            res = i->msg_len;
            found = true;
        }
    }
    return res;
}

void Networking::remove_sent(int sockfd, int msg_t)
{
    bool found = false;
    auto it = send_queues[sockfd].begin();
    for (auto i = it; i != send_queues[sockfd].end() && !found; ++i)
    {
        if (i->msg_type == msg_t)
        {
            found = true;
            it = i;
        }
    }
    send_queues[sockfd].erase(it);
}