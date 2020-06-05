#ifndef TIN_LOGINHANDLER_H
#define TIN_LOGINHANDLER_H

#include <iostream>
#include <string>
#include "TextFileHandler.h"
#include "FileIOHandler.h"

class LoginHandler
{
private:
    struct LoginInfo
    {
        std::string login;
        std::string passwd;
        LoginInfo(std::string log, std::string pass) : login(std::move(log)), passwd(std::move(pass)) {};
        bool is_lexically_correct() const;
        //bool user_exists();
    };
    std::string users_info_buff;
    TextFileHandler users_info;
    bool is_buff_empty();
    int user_exists(std::string const &login);
    std::string get_users_info_filename();
    int find_user(std::string const &login);
    bool cmp_login_info(LoginInfo const &login_inf, int login_pos);
    //bool is_lexically_correct();
    LoginInfo parse_login_info_from_string(std::string const &data);
    int is_login_info_correct(LoginInfo const &login_inf);
    void add_new_client(LoginInfo const &login_inf);
    void close_buff();
public:
    LoginHandler()
    {
        /*ctor with files handler not working well, mess with sockets*/
        std::string fn = "./user_data/users.txt";
        //FileIOHandler file_handler;

        //if (!file_handler.file_exists(fn)/*FileIOHandler().file_exists(fn)*/)
        //{
            //std::cout << "file doesn't exist" << std::endl;
            //file_handler.create_file("./user_data/", "users.txt");
            //FileIOHandler().create_file("./user_data/", "users.txt"); // ugly af
        //}
        users_info.add_filename(fn);
    }
    enum login_consts {NON_EXISTENT = -1, INCORRECT = 0, CORRECT = 1};
    int check_login(std::string const &bytes);

};


#endif //TIN_LOGINHANDLER_H
