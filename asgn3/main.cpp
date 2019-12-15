// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_pair = xpair<const string,string>;
using str_str_map = listmap<string, string, xless<string>>;
typedef listmap<string, string, xless<string>>::iterator lmap_str_itor;

void trim(string &str){
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");

    if(string::npos == start || string::npos == end) 
        str = "";
    else 
        str = str.substr(start,end-start+1);
}

    listmap<string, string, xless<string>> test;
void parsefile(const string &infilename, istream &infile){
    for(int i = 1;; ++i) {
        try {
            string str;
            getline (infile, str);
            if(infile.eof()) break; //if end of file, stop loop
            trim(str);
            cout << infilename << ": " << i << ": " << str <<endl;
            
            if (str.size() == 0){}
            
            else if (str.at(0) == '#'){}
            
            else if (str.compare("=") == 0){ 
                DEBUGF('a', "print everything");
                lmap_str_itor it = test.end();
                while(it != test.begin()){
                    --it;
                    cout << it->first << " = "
                         << (*it).second << endl;
                }
            }
            
            else if(str.find_first_of("=", 0) == string::npos){
                lmap_str_itor it2 = test.find(str);
                if(it2 == test.end())
                    cout << str <<": key not found" << endl;
                else
                    cout << (*it2).first << " = "
                         << (*it2).second << endl;
            }
            
            else if(str.find_first_of("=", 0) == str.size() - 1){
                DEBUGF('a',"key =");
                string temp = str.substr(0, str.size() - 1);
                trim(temp);
                lmap_str_itor it3 = test.find(temp);
                if(it3 != test.end()) 
                    it3.erase();
            }
            
            else if(str.at(0) == '='){
                DEBUGF('a',"= value");
                string temp = str.substr(1, str.size());
                lmap_str_itor it4 = test.end();
                while(it4!=test.begin()){
                    --it4;
                    if(temp.compare(it4->second)==0)
                        cout << it4->first << " = "
                             << it4->second << endl;
                }
            }
            
            else{
                DEBUGF('a',"key = value");
                string temp = str.substr(0, str.find_first_of("=", 0));
                string temp2 = str.substr(
                           str.find_first_of("=", 0) + 1, str.size());
                trim(temp); 
                trim(temp2);
                str_str_pair pr(temp, temp2);
                test.insert(pr);
                cout << temp << " = " << temp2 <<endl;
            }
            DEBUGF ('m', str);

        } catch(runtime_error e){
            complain() << infilename << ":" << i
                       << ": " << e.what() <<endl;
        }
    }
    DEBUGF ('m', infilename << "EOF");
}

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

int main (int argc, char** argv) {
    sys_info::execname (argv[0]);
    scan_options (argc, argv);
    
    bool ran = false;
    for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
        ran = true;
        string filename = *argp;
        if (filename.compare("-") == 0){
            parsefile("-", cin);
            continue;
        }
        ifstream in(filename);
        if (in.fail()) {
            syscall_error(filename);
            continue;
        }
        DEBUGF ('m', filename << " =>");
        parsefile(filename, in);
        in.close();
    }
    if(!ran) parsefile("-", cin);
    
    return EXIT_SUCCESS;
}
