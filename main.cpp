#include <iostream>
#include <filesystem>
// #include <fstream>

#include <string>
#include <vector>

namespace fs = std::filesystem;

enum class Flags {
    VERBOSE = 0, // write out exactly what is going on
    LOGFILE, // dont print to STDOUT and use an specified logfile instead
    OVERRIDE, // rewrite everything at destination (formaly: delete everything in destination before continuing)
    CREATE_DUMMY, // create dummyfiles if a link goes into nowhere
    UPDATE, // update dierectly (update script must be at ./update.sh OR .\update.bat) (more spcification in the implementation)
    SCRIPT, // don't ask questions
    number
};

struct Args {
    std::array<bool, static_cast<size_t>(Flags::number)> flags;
    fs::path logfile;
    fs::path source;
    fs::path dest;
};

int main (const int argc, const char** argv) {
    
    
    Args arguments;
    
    {
        std::vector<std::string> args;
        for (int i = 1; i < argc; i++) {
            std::string tmp(argv[i]);
            args.push_back(tmp);
        }
        
        int i = 0;
        for (; i < args.size(); i++) {
            
            
            std::string* akt_arg = &args[i];
            
            if (akt_arg->length() == 0) // should not occoure, but save is save
                continue;
            
            if ((*akt_arg)[0] == '-') {
                if (akt_arg->length() == 1) // a single '-' is read as end of flag setting (so the source/dest paths after could start with '-')
                    break;
                
                if ((*akt_arg)[1] == '-') { // the --flag flags
                    
                } else { // the -f flags
                    for (int j = 1; j < akt_arg->length(); j++) {
                        switch ((*akt_arg)[j]) {
                            case 'v':
                                arguments.flags[static_cast<size_t>(Flags::VERBOSE)] = true;
                                break;
                            
                            case 'o':
                                arguments.flags[static_cast<size_t>(Flags::OVERRIDE)] = true;
                                break;
                            
                            case 'd':
                                arguments.flags[static_cast<size_t>(Flags::CREATE_DUMMY)] = true;
                                break;
                            
                            case 'u':
                                arguments.flags[static_cast<size_t>(Flags::UPDATE)] = true;
                                break;
                            
                            case 's':
                                arguments.flags[static_cast<size_t>(Flags::SCRIPT)] = true;
                                break;
                            
                            case 'l':
                                arguments.flags[static_cast<size_t>(Flags::LOGFILE)] = true;
                                arguments.logfile = "./phraser.log";
                                break;
                            
                            default:
                                std::cerr << "invalide single-char flag '" << (*akt_arg)[j] << "' is ignored!";
                        }
                    }
                }
                
                
            } else {
                break;
            }
        }
    }
    
//     sf::path::format::generic_format
    
//     for () {
//         
//     }
}
