#include "ClientsMonitor.h"

void ClientsMonitor::add_new_cli(int sockfd)
{
    clients_state.insert(std::make_pair(sockfd, std::make_pair(std::make_pair(IDLE, 0), std::make_pair(HEADER_TO_SEND, Header::consts::header_size))));
}

int ClientsMonitor::get_to_write_connections_number()
{
    int count = 0;
    for (auto &elem : clients_state)
    {
        auto sockfd = elem.first;
        if (get_socket_write_state(sockfd) == HEADER_TO_SEND || get_socket_write_state(sockfd) == HEADER_SENT)
            count += 1;
    }
    return count;
}

bool ClientsMonitor::is_logged(int sockfd)
{
    return logged_clients.find(sockfd) != logged_clients.end();
}

int ClientsMonitor::get_socket_read_state(int sockfd)
{
    if (!clients_state.count(sockfd))
        return -2;
    return clients_state[sockfd].first.first;
}

int ClientsMonitor::get_socket_read_bytes_number(int sockfd)
{
    if (!clients_state.count(sockfd))
        return -2;
    return clients_state[sockfd].first.second;
}

int ClientsMonitor::get_socket_write_state(int sockfd)
{
    if (!clients_state.count(sockfd))
        return -2;
    return clients_state[sockfd].second.first;
}

int ClientsMonitor::get_socket_write_bytes_number(int sockfd)
{
    if (!clients_state.count(sockfd))
        return -2;
    return clients_state[sockfd].second.second;
}

void ClientsMonitor::set_socket_read_state(int sockfd, int val)
{
    clients_state[sockfd].first.first = val;
}

void ClientsMonitor::set_socket_read_bytes_number(int sockfd, int val)
{
    clients_state[sockfd].first.second = val;
}

void ClientsMonitor::set_socket_write_state(int sockfd, int val)
{
    clients_state[sockfd].second.first = val;
}

void ClientsMonitor::set_socket_write_bytes_number(int sockfd, int val)
{
    clients_state[sockfd].second.second = val;
}

int ClientsMonitor::get_existing_conns()
{
    return clients_state.size();
}

std::vector<int> ClientsMonitor::get_write_descriptors()
{
    std::vector<int> res;
    for (auto &elem : clients_state)
    {
        auto sockfd = elem.first;
        if (get_socket_write_state(sockfd) == HEADER_TO_SEND || get_socket_write_state(sockfd) == HEADER_SENT)
            res.push_back(sockfd);
    }
    return res;
}