#include "Statement.h"

std::string Statement::get_string_rep() const
{
    return std::to_string(statement_info);
}

int Statement::get_message_type() const
{
    return 1;
}