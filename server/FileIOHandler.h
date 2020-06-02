#ifndef TIN_FILEIOHANDLER_H
#define TIN_FILEIOHANDLER_H


#include <fstream>

class FileIOHandler
{
private:
    //std::fstream fs;
public:
    void create_file(std::string const &path, std::string const &name);
    void open_file();
    void read_file();
    void find(std::string const &str);
    std::string read_to_buffer(std::string const &fn);
};


#endif //TIN_FILEIOHANDLER_H
