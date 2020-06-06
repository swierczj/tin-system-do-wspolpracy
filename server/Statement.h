#ifndef TIN_STATEMENT_H
#define TIN_STATEMENT_H
#include "Message.h"

class Statement : public Message
{
public:
    enum Info {KEEP_ALIVE = 0, LOGIN_REQ = 1, LOGIN_ACC = 2, LOGIN_REJ = 3, LOG_OUT = 4, WORK_END = 5, PUB_KEY_REQ = 6
              , FILE_LIST_REQ = 7, ACCOUNT_LIST_REQ = 8, ACCOUNT_CHANGE_ACC = 9, ACCOUNT_CHANGE_REJ = 10};
    Statement(int info) : statement_info(info) {}
    std::string get_string_rep() const override;
    int get_message_type() const override;
private:
    int statement_info;
};


#endif //TIN_STATEMENT_H
