#ifndef TIN_STATEMENT_H
#define TIN_STATEMENT_H
#include "Message.h"

class Statement : public Message
{
public:
    std::string get_string_rep() override;
};


#endif //TIN_STATEMENT_H
