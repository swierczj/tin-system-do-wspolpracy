#include "ClientID.h"

std::string ClientID::get_string_rep() const
{
    return std::to_string(id);
}

int ClientID::get_message_type() const
{
    return Message::CLIENT_ID;
}