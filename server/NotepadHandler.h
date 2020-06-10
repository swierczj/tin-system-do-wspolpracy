#ifndef TIN_NOTEPADHANDLER_H
#define TIN_NOTEPADHANDLER_H

//#include <filesystem>
#include <map>
//#include <chrono>
#include <set>
#include "TextFileHandler.h"
#include "FileIOHandler.h"
#include "Note.cpp"

class NotepadHandler
{
private:
    bool opened;
    TextFileHandler text_files;
    //unsigned int active_users;
    std::set<int> contributors;
    Note notepad;
    std::set<int> to_fetch_changes;
    std::map<int, std::string> changes_buffs;
    struct PendingChanges
    {
        int from;
        int change_nbytes;
        PendingChanges() : from(-1), change_nbytes(-1) {}
        PendingChanges(int fr, int nbytes) : from(fr), change_nbytes(nbytes) {}
        bool equals(PendingChanges const &pc) {return from == pc.from && change_nbytes == pc.change_nbytes;}
    };
    std::map<int, std::list<PendingChanges>> changes_tracker;
    int changes_left;
    void init(std::string const &fn);
    void keep_track_of_changes(int sockfd, int nbytes);
    PendingChanges last_change;
    void remove_extracted_changes(PendingChanges &pc, int sockfd);
public:
    NotepadHandler() : opened(false), notepad(-1) {}
    void open();
    void add_contributor(int sockfd);
    bool is_active(int sockfd);
    void add_client(int sockfd);
    void edit_contents(std::string const &changes);
    //void init()
    void store_changes(int sockfd, std::string const &edit);
    bool is_to_fetch_changes(int sockfd);
    int get_active_users_number();
    int get_notepad_current_size();
    std::set<int> get_contributors();
    std::string get_file_as_str();
    std::string get_stored_changes(int sockfd);
    int get_changes_left();
    std::string get_changes(int sockfd);
    void remove_sent_changes(int sockfd);
    void remove_contributor(int sockfd);
    void save_to_file();

    std::string get_str();
};


#endif //TIN_NOTEPADHANDLER_H
