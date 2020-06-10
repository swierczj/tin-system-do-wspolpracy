#ifndef TIN_TEXTFILEHANDLER_H
#define TIN_TEXTFILEHANDLER_H

#include <string>
#include <vector>
#include <algorithm>

class TextFileHandler
{
private:
    std::vector<std::string> filenames;
    //FileIOHandler io_handler;
    std::string parse_name(std::string const &fn);
public:
    void add_filename(std::string const &fn);
    void delete_filename(std::string const &fn);
    std::string get_filenames_in_str(char delimiter);
    int get_size();
    std::string get_filename_at_pos(int index);
    std::string get_recent_fn();
};


#endif //TIN_TEXTFILEHANDLER_H
