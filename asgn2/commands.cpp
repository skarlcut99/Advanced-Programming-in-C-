// $Id: commands.cpp,v 1.18 2019-10-08 13:55:31-07 - - $

#include <iostream>
#include <stdio.h>
#include <string>
#include <cstddef>

#include "commands.h"
#include "debug.h"

using namespace std;

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}
void fn_cat (inode_state& state, const wordvec& words){
    if (words.size() == 1)
       throw command_error("fn_cat: no ags specified");
    else if (words.size() > 1) 
    {
       shared_ptr <directory> dir = 
       dynamic_pointer_cast<directory>(state.get_cwd()->get_contents());
       for(unsigned int i = 1; i < words.size(); i++) {
         inode_ptr content = dir->get_file(words[i]);
         if(content == inode_ptr()) 
            cout << "cat: " << words[i] << " no such file exists.\n";
         else 
         {
            shared_ptr <plain_file> text = 
            dynamic_pointer_cast<plain_file>(content->get_contents());
            cout << text->readfile() << endl;
         }
      }
   }
   else
        cout << "cat: expected a file to cat.\n";
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}
void navigate(bool& success, shared_ptr <directory> current_dir, 
         const wordvec& dirnames){
   inode_ptr next = dynamic_pointer_cast <inode> (current_dir);
   success = true;
   for(unsigned int i = 0; i < dirnames.size(); i++){
      next = 
      dynamic_pointer_cast<inode>(current_dir->get_file(dirnames[i]));
      if(next == inode_ptr()){
         success = false;
         break;
      }
      if(next->get_type()) { 
        current_dir = 
           dynamic_pointer_cast <directory>(current_dir->get_file
                                      (dirnames[i])->get_contents());
      }
      else
      {
         success = false;
         break;
      }
   }
}

void fn_cd (inode_state& state, const wordvec& words){
   if(words.size() == 1)
      state.set_cwd(state.get_root());
   else if(words.size() == 2)
   {
      shared_ptr <directory> current
            = dynamic_pointer_cast<directory>
                  (state.get_cwd()->get_contents());
      inode_ptr next = 
          dynamic_pointer_cast <inode> (current);
      wordvec dir = split (words[1], "/");
      bool success = true;
      for(unsigned int x = 0; x < dir.size(); x++)
      {
         next = dynamic_pointer_cast <inode>
                    (current->get_file(dir[x]));
         if(next == inode_ptr())
         {
                success = false;
                break;
         } 
         if(next->get_type())
         { 
            current = dynamic_pointer_cast <directory>
            (current->get_file(dir[x])->get_contents());
         }
         else 
         {
             success = false;
             break;
         }
      }
      if(success)
         state.set_cwd(next);
      else
         cout << "cd: cannot navigate to specified dir...\n";
      if(words[1] == "/")
         state.set_cwd(state.get_root());
   }
   else
      cout << "cd: invalid argumenets given\n";
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}

void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   exit_status exit_;
   if (words.size() == 1 || words[1] == "0")
      exit_.set(0);
   else if (atoi(words[1].c_str()))
      exit_.set(atoi(words[1].c_str()));
   else
      exit_.set(127);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
    if(words.size() <= 2)
    {
       shared_ptr <directory> dir = 
                dynamic_pointer_cast <directory> 
                (state.get_cwd()->get_contents());
       cout << dir->printentry();
    }
    else throw command_error("fn_ls:invalid number of args");
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

void fn_lsr (inode_state& state, const wordvec& words){
    shared_ptr <directory> dir = 
                dynamic_pointer_cast<directory>(
                state.get_cwd()->get_contents());
    for(unsigned int x = 1; x < words.size(); x++)
    {
        wordvec parse = split(words[x], "/");
        wordvec::iterator iter = parse.begin();
        dir = dynamic_pointer_cast<directory>(
                state.get_cwd()->get_contents());
        unsigned int size = 0;
        inode_ptr out;
        if(words[x] == "/")
        {
            shared_ptr <directory> dir = 
            dynamic_pointer_cast<directory>
                (state.get_root()->get_contents());
            vector <int> *done = new vector <int> (0);
            string str = dir->printentryrec("", done);
            cout << str.substr(str.rfind("/:"), 
                                            str.length() - 1);
            delete(done);
        }
        else
        {
            try {
                while(size < parse.size())
                {
                    if(dir->file_exists(*iter))
                        out = dir->get_file(*iter);
                    else
      throw command_error("fn_lsr: directory does not exist " + *iter);
                    if(!out->get_type())
    throw command_error("fn_lsr: expected a directory and not a file");
                    else
                    {
                        iter++;
                        size++;
                        dir = dynamic_pointer_cast<directory>
                                              (out->get_contents());
                    }
                }
                dir = 
                dynamic_pointer_cast<directory>(out->get_contents());
                vector <int> *done = new vector<int>
                                               (out->get_inode_nr());
                string str = dir->printentryrec("", done);
                cout << str.substr(str.rfind(
                        dir->getfilename() + ":"), str.length() - 1);
                delete(done);
            }
            catch(std::out_of_range) {
                cout << "fn_lsr: directory does not exist " + *iter 
                     << endl;
            }
        }
        size = 0;
    }
    if( words.size() == 0 )
        throw command_error("fn_lsr:invalid number of args");
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words){
   if(words.size() > 2) {
      wordvec parse = split(words[1], "/");
      wordvec::iterator iter = parse.begin();
      shared_ptr <directory> dir = dynamic_pointer_cast<directory>(
              state.get_cwd()->get_contents());
      unsigned int size = 0;
      if(parse.size() < 2 )
      {
         if(dir->get_file(words[1])  == inode_ptr())
            dir->createfile(words[1], 
                    wordvec (words.begin() + 2, words.end()));
         else
            cout << "make: " << words[1] << " file already exists.\n";
      }
      else
      {
         try {
            while(size < (parse.size() - 1 ))
            {
               inode_ptr out;
               if(dir->file_exists(*iter))
                  out = dir->get_file(*iter);
               else
           throw command_error("fn_make: directory does not exist");
               if(!out->get_type())
  throw command_error("fn_make: expected a directory and not a file");
               else
               {
                  iter++;
                  size++;
                  dir = dynamic_pointer_cast<directory>
                                     (out->get_contents());
               }
            }
            if(dir->get_file(*iter)  == inode_ptr())
               dir->createfile(*iter, 
                        wordvec (words.begin() + 2, words.end()));
            else
               cout << "make: " << words[1] 
                    << " file already exists.\n";
         }
         catch(std::out_of_range){
           throw command_error("fn_make:cannot find directory");
         }
      }
   }
   else 
      cout << "make: expected more than one argument\n";
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   if(words.size() == 1) throw command_error("fn_mkdir: no args");
   wordvec parse = split(words[1], "/");
   wordvec::iterator iter = parse.begin();
   shared_ptr <directory> dir = 
            dynamic_pointer_cast<directory>(
                state.get_cwd()->get_contents());
   unsigned int size = 0;
   if(words.size() == 2) {
      if(parse.size() < 2 )
      {
         if(!dir->file_exists(words[1])) 
            dir->createdir(words[1]);
         else
            throw command_error( "fn_mkdir: " + words[1] 
                        + " file already exists.");
      }
      else
      {
         try 
         {
            while(size < parse.size() - 1)
            {
               inode_ptr out;
               if(dir->file_exists(*iter))
                  out = dir->get_file(*iter);
               else
   throw command_error("fn_mkdir: directory does not exist " + *iter);
               if(!out->get_type())
  throw command_error("fn_mkdir: expected a directory and not a file");
               else
               {
                  iter++;
                  size++;
                  dir = dynamic_pointer_cast<directory>
                            (out->get_contents());
               }
            }
            if(!dir->file_exists(*iter))
               dir->createdir(*iter);
            else
            {
               throw command_error( "fn_mkdir: " + *iter
                     + " file already exists.");
            }
         }
         catch(std::out_of_range)
         {
            throw command_error("fn_mkdir: "
                         "cannot find directory");
         }
      }
   }
   else throw command_error("fn_mkdir: invalid numbr of args");
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_prompt (inode_state& state, const wordvec& words){
   string str = "";
   if(words.size() >= 2)
   {
      for(unsigned int x = 0; x < words.size(); x++)
      {
         str += words[x];
         if(x < (words.size() - 1))
            str += " ";
      }
      state.set_prompt(str);
   }
   else
      cout << "prompt: wrong number of arguments passed\n";
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

string get_name(shared_ptr <directory> current){
    string str = current->getfilename();
    if(str.size() == 0)
       str = "/";
    return str;
}

void fn_pwd (inode_state& state, const wordvec& words){
   if(words.size()==1){ 
      shared_ptr <directory> dir 
               = dynamic_pointer_cast<directory> 
               (state.get_cwd()->get_contents());
      string str = get_name(dir);
      cout << str << endl;
   }
   else throw command_error("fn_pwd:invalid num of args"); 
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rm (inode_state& state, const wordvec& words){
   shared_ptr <directory> dir 
        = dynamic_pointer_cast<directory>
            (state.get_cwd()->get_contents());
   if(words.size() == 2){
      auto temp = dir->get_file(words[1]);
      auto curr 
          = dynamic_pointer_cast<directory> (temp->get_contents());
      if(!temp->get_type() || curr->num_files() == 0){
         if(temp->get_type())
            curr->clear_files();
         dir->remove(words[1]);
      }
      else 
         cout << "rm: " << words[1] << ": is a non-empty directory.\n";
   }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   shared_ptr <directory> dir = 
            dynamic_pointer_cast<directory>(
                state.get_cwd()->get_contents());
   for(unsigned int x = 1; x < words.size(); x++)
   {
      wordvec parse = split(words[x], "/");
      wordvec::iterator iter = parse.begin();
      dir = dynamic_pointer_cast<directory>(
                state.get_cwd()->get_contents());
      unsigned int size = 0;
      inode_ptr out;
      if(parse.size() <= 2 )
      {
         if(dir->file_exists(*parse.begin())) {
            shared_ptr<directory> prev = dir;
            dir = dynamic_pointer_cast<directory>
                    (dir->get_file(words[x])->get_contents());
            dir->clear_files();
            prev->remove(*parse.begin());
         }
         else
            cout << "fn_rmr: " + words[x] 
                 + " does not exists.\n";
      }
      else
      {
         try {
            while(size < parse.size() - 1)
            {
               if(dir->file_exists(*iter))
                  out = dir->get_file(*iter);
               else
      throw command_error("fn_rmr: directory does not exist " + *iter);
               if(!out->get_type())
  throw command_error("fn_rmr: expected a directory and not a file");
               else
               {
                  iter++;
                  size++;
                  dir = dynamic_pointer_cast<directory> 
                       (out->get_contents());
                }
            }
            if(dir->file_exists(*iter)) 
            {
               shared_ptr<directory> prev = dir;
               dir = dynamic_pointer_cast<directory>
                        (dir->get_file(words[x])->get_contents());
               dir->clear_files();
               prev->remove(*iter);
            }
            else 
                cout << "fn_rmr: " + *iter + " does not exists.\n";
         }
         catch(std::out_of_range) {
            cout << "fn_rmr: " + words[x] 
                     + " cannot find directory";
         }
      }
   }
}
