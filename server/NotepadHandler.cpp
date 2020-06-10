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
    changes_tracker.insert(std::make_pair(sockfd, std::list<PendingChanges>()));
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
    std::cout << "in edit contents" << std::endl;
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
    std::cout << "in storing changes" << std::endl;
    changes_buffs[sockfd].append(edit);
    std::cout << "appended" << std::endl;
    keep_track_of_changes(sockfd, edit.size());
}

bool NotepadHandler::is_to_fetch_changes(int sockfd)
{
    std::cout << "is_to_fetch" << std::endl;
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
    if (changes_left > 0 && !recent_change.equals(last_change))
        return "";
    // if all changes from last "round" sent
    if (changes_left == 0)
    {
        last_change = recent_change;
        //changes_left = contributors.size() - 1;
    }
    std::string res = changes_buffs[editor_id].substr(0, recent_change.change_nbytes);
    //remove_extracted_changes(recent_change, sockfd);
    return res;
}

void NotepadHandler::keep_track_of_changes(int sockfd, int nbytes)
{
    // add new change to send for clients except sender
    if (changes_tracker.empty())
        std::cout << "changes tracker empty" << std::endl;
    for (auto & elem : changes_tracker)
    {
        std::cout << "for range, elem: " << elem.first << ", nbytes: " << nbytes << std::endl;
        if (elem.first != sockfd)
            elem.second.push_back(PendingChanges(sockfd, nbytes));
        else
            std::cout << "not emplaced" << std::endl;
    }
}

void NotepadHandler::remove_extracted_changes(PendingChanges &pc, int sockfd)
{
    int editor_id = pc.from;
    changes_buffs[editor_id].erase(0, pc.change_nbytes);
    changes_tracker[sockfd].pop_front();
}

void NotepadHandler::remove_sent_changes(int sockfd)
{
    int editor_id = changes_tracker[sockfd].begin()->from;
    int changes_len = changes_tracker[sockfd].begin()->change_nbytes;
    changes_tracker[sockfd].pop_front();
    changes_left -= 1;
    // if last change of round then erase sent edit from buff
    if (changes_left == 0)
    {
        changes_buffs[editor_id].erase(0, changes_len);
        // if editor exited but buffer had messages
        if (!is_active(editor_id))
            changes_buffs.erase(editor_id);
        changes_left = get_active_users_number() - 1;
    }
}

void NotepadHandler::remove_contributor(int sockfd)
{
    contributors.erase(sockfd);
    // if no pending changes from client, then delete buffer
    if (changes_buffs[sockfd].empty())
        changes_buffs.erase(sockfd);
    // else store changes to send to ther clients

    // if exist pending changes to removed client
    if (changes_tracker[sockfd].size() > 0)
    {
        // if client didn't receive last msg
        if (changes_tracker[sockfd].front().equals(last_change) && changes_left > 0)
            changes_left -= 1;
        // if received or ch_left == 0 then do nothing just erase
    }
    changes_tracker.erase(sockfd);
}

void NotepadHandler::save_to_file()
{
    auto edited_contents = notepad.toString();
    FileIOHandler().write_from_buffer(text_files.get_recent_fn(), edited_contents);
}

std::string NotepadHandler::get_str()
{
    return notepad.toString();
}