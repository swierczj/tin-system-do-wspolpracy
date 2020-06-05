#ifndef TIN_FILEIOHANDLER_H
#define TIN_FILEIOHANDLER_H


#include <fstream>
#include <iostream>

class FileIOHandler
{
private:
    //std::fstream fs;
public:
    void create_file(std::string const &fn);
    //void open_file();
    //void read_file();
   // void find(std::string const &str);
    std::string read_to_buffer(std::string const &fn);
    bool file_exists(std::string const &fn);
    //std::string get_from_pos_to_endline(int pos, std::string const &fn);
    void write_from_buffer(std::string const &fn, std::string const &buff);
    bool is_empty(std::string const &fn);
};


#endif //TIN_FILEIOHANDLER_H
