#include "Header.h"

std::string Header::get_string_rep() const
{
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

int Header::get_msg_len() { return msg_len; }

int Header::get_msg_type() { return msg_type; }

int Header::get_message_type() const
{
    // -1 for Header
    return -1;
}

Header parse_from_string(std::string const &str)
{
    std::cout << "header: " << str << " size: " << str.size() /*<< "and exception check values: " << (str.size() != Header::header_size) << " and 2nd: " << std::all_of(str.begin(), str.end(), ::isdigit)*/ << std::endl;
    if (str.size() != Header::header_size || !std::all_of(str.begin(), str.end(), ::isdigit))
        throw std::underflow_error("wrong header");
    std::string msg_type_part = str.substr(0, Header::msg_type_len);
    std::string msg_len_part = str.substr(Header::msg_type_len, Header::header_size);
    int msg_type = std::stoi(msg_type_part);
    int msg_len = std::stoi(msg_len_part);
    std::cout << "msg type: " << msg_type << std::endl << "msg len: " << msg_len << std::endl;
    return Header(msg_type, msg_len);
}
