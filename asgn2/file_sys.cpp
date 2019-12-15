// $Id: file_sys.cpp,v 1.7 2019-07-09 14:05:44-07 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"
#include "commands.h" 

int inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

void inode_state::set_root(inode_ptr that) {
    root = that;
};

inode_ptr inode_state::get_root() {
    return root;
};

void inode_state::set_cwd(inode_ptr that) {
    cwd = that;
};

inode_ptr inode_state::get_cwd() {
    return cwd;
};

void inode_state::set_prompt(string that) {
    prompt_ = that;
}

base_file_ptr inode::get_contents()
{
    return contents;
}



void inode::set_contents(base_file_ptr that)
{
    contents = that;
}

inode_state::inode_state() {
   root = make_shared<inode>(file_type::DIRECTORY_TYPE);
   cwd = root;
   shared_ptr <directory> base_dir 
       = dynamic_pointer_cast<directory> 
        (root->get_contents());
   base_dir->setfilename("");
   base_dir->addentry(".", root);
   base_dir->addentry("..", root);
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
}

inode_state::~inode_state() {
    cwd = nullptr;
    shared_ptr <directory> base_dir 
       = dynamic_pointer_cast<directory> 
        (root->get_contents());
    base_dir->clear_files();
    root = nullptr;
}

const string& inode_state::prompt() { return prompt_; }

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           is_dir = false;
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           is_dir = true;
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

bool inode::get_type() const {
    return is_dir;
}

file_error::file_error (const string& what):
            runtime_error (what) {
}

int directory::num_files() {
    return dirents.size() - 2;
}

string base_file::getfilename()
{
    return filename;
}

void base_file::setfilename(const string& name)
{
    filename = name;
}

size_t plain_file::GetSize() {
   DEBUGF ('i', "size = " << size);
   return size;
}

void plain_file::SetSize(size_t newSize) {
    size = newSize;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   data = words;
   DEBUGF ('i', words);
}

void plain_file::remove (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkdir (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkfile (const string&) {
   throw file_error ("is a plain file");
}

void directory::SetSize(size_t newSize) {
    size = newSize;
}

const wordvec& directory::readfile() const {
   throw file_error ("is a directory");
}

void directory::writefile (const wordvec&) {
   throw file_error ("is a directory");
}

inode_ptr directory::get_file (const string& filename) {
    try {    
        return dirents.at(filename);  
    }  
    catch (const std::out_of_range& e) { 
        return inode_ptr();
    }
}

void directory::addentry (const string& filename,
                                    const inode_ptr& file) {
    dirents[filename] = file;
}

void directory::createfile (const string& filename,
                                    const wordvec& newdata) {
    inode_ptr new_file =  make_shared<inode>(file_type::PLAIN_TYPE);
    size_t size = 0;
    for(unsigned int i = 0; i < newdata.size(); i++)
    {
        size += newdata[i].length();
        if(i < (newdata.size() - 1))
        {
            size++;
        }
    }
    addentry(filename, new_file);
    shared_ptr <plain_file> file 
      = dynamic_pointer_cast<plain_file> (new_file->get_contents());
    file->SetSize(size);
    file->setfilename(getfilename() + "/" + filename);
    file->writefile(newdata);
}

void directory::createdir (const string& filename) {
    inode_ptr new_file 
        =  make_shared<inode>(file_type::DIRECTORY_TYPE);
    addentry(filename, new_file);
    shared_ptr <directory> file 
        = dynamic_pointer_cast<directory> (new_file->get_contents());
    file->setfilename(getfilename() + "/" + filename);
    file->dirents["."] = new_file;
    file->dirents[".."] = dirents["."];
}

void directory::clear_files () {
    for (std::map<const string, 
            inode_ptr>::iterator it=dirents.begin();
                                 it!=dirents.end(); ++it)
    {
        if((it->first != ".") && (it->first != "..") 
                                    && (it->second->get_type()))
        {
            shared_ptr<directory> dir = 
                dynamic_pointer_cast<directory> 
                                        (it->second->get_contents());
            dir->clear_files();
        }
        
    }
    dirents.clear();
}

bool directory::file_exists(const string& filename) {
    try {
        dirents.at(filename);
        return true;
    }
    catch(std::out_of_range) {
        return false;
    }
}

string directory::printentry() {
    string str = "";
    for (std::map<const string, 
            inode_ptr>::iterator it=dirents.begin();
                                 it!=dirents.end(); ++it)
    {
        if(it->second->get_type() == true)
        {
            shared_ptr <directory> item 
             = dynamic_pointer_cast<directory> 
                    (it->second->get_contents());
            str.append(to_string(it->second->get_inode_nr()));
            str.append("   ");
            str.append(to_string(item->GetSize()));
            str.append("   ");
            str.append(it->first);
            str.append("\n");
        }
        else 
        {
         shared_ptr <plain_file> item 
         = dynamic_pointer_cast<plain_file> 
                  (it->second->get_contents());
            str.append(to_string(it->second->get_inode_nr()));
            str.append("   ");
            str.append(to_string(item->GetSize()));
            str.append("   ");
            str.append(it->first);
            str.append("\n");
        }
    }
    return str;
}

string directory::printentryrec(string content, vector <int> *visited) {
    string str = content;
    bool found = false;
  
    if(getfilename() == "")
    {
        str += "/:\n";
    }
    else
    {
        str = str + getfilename() + ":\n";
    }
    str = str + printentry();
    std::map<const string,inode_ptr>::iterator it=dirents.begin();
    visited->push_back(it->second->get_inode_nr());
    it++;
    visited->push_back(it->second->get_inode_nr());
    it++;
    while (it!=dirents.end())
    {
        for(unsigned int i = 0; i < visited->size(); i++)
        {
            if(visited->at(i) == it->second->get_inode_nr())
            {
                found = true;
                break;
            }
        }
        if(!found)
        {
            visited->push_back(it->second->get_inode_nr());
            if(it->second->get_type() 
                  && it->first != "." && it->first != "..")
            {
                shared_ptr <directory> dir 
                  = dynamic_pointer_cast<directory> 
                                (it->second->get_contents());
                str += dir->printentryrec(str, visited);
            }
        }
        ++it;
        found = false;
    }
    return str;
}

size_t directory::GetSize() {
   DEBUGF ('i', "size = " << GetSize());
   return dirents.size();
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
   dirents.erase(filename);
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   if (get_file(dirname) == inode_ptr())
   {
        dirents.at(dirname) = inode_ptr();
   }
   return nullptr;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   dirents.at(filename) = inode_ptr();
   return nullptr;
}
