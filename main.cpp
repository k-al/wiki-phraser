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
    fs::path update_script;
    fs::path source;
    fs::path dest;
    
    bool get (const Flags& flag) {
        return this->flags[static_cast<size_t>(flag)];
    }
};

#include "string_helper.h"

static Args arguments;
static Logger logger;

int main (const int argc, const char** argv) {
    
    
    
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
                if (akt_arg->length() == 1) { // a single '-' is read as end of flag setting (so the source/dest paths after could start with '-')
                    i++;
                    break;
                }
                
                if ((*akt_arg)[1] == '-') { // the --flag flags
                    std::string flag = akt_arg->substr(2);
                    if (flag == "update") {
                        if ((i+1) < args.size() && args[i+1][0] != '-') {
                            i++;
                            arguments.flags[static_cast<size_t>(Flags::UPDATE)] = true;
                            arguments.update_script = chomp(args[i]);
                        } else {
                            std::cerr << "WARNIG: word-flag '--update' was used, but no update script was specified!\n  continuing by ignoring the flag.\n";
                        }
                    }
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
                                arguments.update_script = "./update.sh";
                                break;
                            
                            case 's':
                                arguments.flags[static_cast<size_t>(Flags::SCRIPT)] = true;
                                break;
                            
                            case 'l':
                                arguments.flags[static_cast<size_t>(Flags::LOGFILE)] = true;
                                arguments.logfile = "./phraser.log";
                                break;
                            
                            case 'h':
                                std::cout << "./phraser [<FLAGS>] [-] <source> <destination>\n\n"
                                          << "Flags:\n"
                                          << "-v\n\tverbose mode\n\n"
                                          << "-h\n\tshow this help\n\n";
                                          
                                return 0;
                            
                            default:
                                std::cerr << "WARNIG: invalide single-char flag '-" << (*akt_arg)[j] << "' is ignored!\n";
                        }
                    }
                }
            } else {
                break;
            }
        }
        
        if (args.size() - i < 2) {
            std::cerr << "ERROR: too few Argumets!\n\tneeded: ./phraser [<Flags>] <Source> <Destionation>\n";
            return 1;
        } else if (args.size() - i > 2) {
            std::cerr << "WARNIG: too many Aruments!\n\tignoring all after ./phraser [<Flags>] <Source> <Destionation>\n";
        }
        
        arguments.source = chomp(args[i]);
        arguments.dest = chomp(args[i+1]);
        
        logger.update(arguments);
        
        logger  << print_flag(arguments.flags[static_cast<size_t>(Flags::VERBOSE)], "verbose")
                << print_flag(arguments.flags[static_cast<size_t>(Flags::SCRIPT)], "is script")
                << print_flag(arguments.flags[static_cast<size_t>(Flags::OVERRIDE)], "override")
                << print_flag(arguments.flags[static_cast<size_t>(Flags::CREATE_DUMMY)], "create dummy")
                << print_flag(arguments.flags[static_cast<size_t>(Flags::LOGFILE)], "file logging", arguments.logfile)
                << print_flag(arguments.flags[static_cast<size_t>(Flags::UPDATE)], "update", arguments.update_script)
                << "source:      " << arguments.source << "\n"
                << "destination: " << arguments.dest << "\n";
        
        if (!arguments.get(Flags::SCRIPT)) {
            logger << "continue? (cancel with STRG+C) ";
            std::cin.ignore();
        }
    }
    
//     sf::path::format::generic_format
    
//     for () {
//         
//     }
}
