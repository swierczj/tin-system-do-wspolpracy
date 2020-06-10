#ifndef TIN_FILEIOHANDLER_H
#define TIN_FILEIOHANDLER_H

#include <filesystem>
#include <fstream>
#include <iostream>

class FileIOHandler
{
public:
    void create_file(std::string const &fn);
    std::string read_to_buffer(std::string const &fn);
    bool file_exists(std::string const &fn);
    void write_from_buffer(std::string const &fn, std::string const &buff);
    bool is_empty(std::string const &fn);
    int get_files_number(std::string const &path);
};


#endif //TIN_FILEIOHANDLER_H
