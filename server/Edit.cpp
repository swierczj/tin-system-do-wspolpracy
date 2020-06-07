#include "Edit.h"

int Edit::get_message_type() const
{
    return Message::EDIT;
}

std::string Edit::get_string_rep() const
{
    return data;
}