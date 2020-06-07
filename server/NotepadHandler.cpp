#include "NotepadHandler.h"

void NotepadHandler::open()
{
    opened = true;
}

void NotepadHandler::add_contributor(int sockfd)
{
    if (!opened)
        open();
    //active_users++;
    contributors.insert(sockfd);
}

bool NotepadHandler::is_active(int sockfd)
{
    return contributors.find(sockfd) != contributors.end();
}

void NotepadHandler::add_client(int sockfd)
{
    //if (!is_active(sockfd))
    //{
        add_contributor(sockfd);
        if (get_active_users_number() == 1)
        {
            int files_count = FileIOHandler().get_files_number("./texts");
            std::string fn = "./texts/id_" + std::to_string(sockfd) + "_" + std::to_string(files_count + 1);
            std::cout << "filename: " << fn << std::endl;
            FileIOHandler().create_file(fn);
            text_files.add_filename(fn);
            //init(fn); //for many files
        }
        else
            to_fetch_changes.insert(sockfd);
    //}
}

void NotepadHandler::edit_contents(const std::string &changes)
{
    notepad.applyChanges(changes);
}

void NotepadHandler::init(std::string const &fn)
{
    std::string file_text = FileIOHandler().read_to_buffer(fn);
    notepad.setBuffers(file_text);
}

void NotepadHandler::store_changes(int sockfd, const std::string &edit)
{
    if (!is_active(sockfd))
        throw std::runtime_error("non existent contributor");
    changes_buffs[sockfd] += edit;
    keep_track_of_changes(sockfd, edit.size());
}

bool NotepadHandler::is_to_fetch_changes(int sockfd)
{
    return to_fetch_changes.find(sockfd) != to_fetch_changes.end();
}

int NotepadHandler::get_active_users_number()
{
    return contributors.size();
}

int NotepadHandler::get_notepad_current_size()
{
    auto tmp = notepad.getFileChanges();
    return tmp.size();
}

std::set<int> NotepadHandler::get_contributors()
{
    return contributors;
}

std::string NotepadHandler::get_file_as_str()
{
    return notepad.getFileChanges();
}

std::string NotepadHandler::get_stored_changes(int sockfd)
{
    return changes_buffs.at(sockfd);
}

int NotepadHandler::get_changes_left()
{
    return changes_left;
}

std::string NotepadHandler::get_changes(int sockfd)
{
    auto recent_change = changes_tracker[sockfd].front();
    int editor_id = recent_change.from;
    if (changes_left > 0 && recent_change.equals(last_change))
        changes_left -= 1;
    else if (changes_left == 0)
    {
        last_change = recent_change;
        changes_left = contributors.size() - 1;
    }
    std::string  res = changes_buffs[editor_id].substr(0, recent_change.change_nbytes);
    remove_extracted_changes(recent_change, sockfd);
    return res;
}

void NotepadHandler::keep_track_of_changes(int sockfd, int nbytes)
{
    for (auto & elem : changes_tracker)
    {
        if (elem.first != sockfd)
            elem.second.emplace_back(PendingChanges(sockfd, nbytes));
    }
}

void NotepadHandler::remove_extracted_changes(PendingChanges &pc, int sockfd)
{
    int editor_id = pc.from;
    changes_buffs[editor_id].erase(0, pc.change_nbytes);
    changes_tracker[sockfd].pop_front();
}