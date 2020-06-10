#include "ClientsMonitor.h"

void ClientsMonitor::add_new_cli(int sockfd)
{
    clients_state.insert(std::make_pair(sockfd, std::make_pair(std::make_pair(IDLE, 0), std::make_pair(HEADER_TO_SEND, Header::consts::header_size))));
}

// get number of clients to whom server must init communication
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

void ClientsMonitor::add_logged_client(int sockfd)
{
    logged_clients.insert(std::make_pair(sockfd, false));
}

void ClientsMonitor::add_rejected_login(int sockfd)
{
    rejected_logins.insert(sockfd);
}

void ClientsMonitor::remove_rejected(int sockfd)
{
    rejected_logins.erase(sockfd);
}

bool ClientsMonitor::is_rejected(int sockfd)
{
    return rejected_logins.find(sockfd) != rejected_logins.end();
}

void ClientsMonitor::update_logged(int sockfd)
{
    auto it = logged_clients.find(sockfd);
    if (it != logged_clients.end())
        it->second = true;
}

bool ClientsMonitor::login_ended(int sockfd)
{
    auto it = logged_clients.find(sockfd);
    return it != logged_clients.end() && it->second == true;
}

ClientsMonitor::LoginState ClientsMonitor::get_login_state(int sockfd)
{
    int msg_type = -2;
    int msg_len = -1;
    int info = -1;
    if (get_socket_read_state(sockfd) == IDLE)
    {
        msg_type = Message::STATEMENT;
        msg_len = get_byte_width(Statement::LOGIN_REQ);
        info = Statement::LOGIN_REQ;
    }
    else if (is_logged(sockfd) && !login_ended(sockfd))
    {
        std::cout << "preparing client id" << std::endl;
        msg_type = Message::CLIENT_ID;
        msg_len = get_byte_width(sockfd);
        info = sockfd;
    }
    else if (is_rejected(sockfd))
    {
        msg_type = Message::STATEMENT;
        msg_len = get_byte_width(Statement::LOGIN_REJ);
        info = Statement::LOGIN_REJ;
    }
    return {msg_type, msg_len, info};
}

void ClientsMonitor::remove_socket(int sockfd)
{
    if (clients_state.find(sockfd) != clients_state.end())
        clients_state.erase(sockfd);
    if (logged_clients.find(sockfd) != logged_clients.end())
        logged_clients.erase(sockfd);
    if (rejected_logins.find(sockfd) != rejected_logins.end())
        rejected_logins.erase(sockfd);
}