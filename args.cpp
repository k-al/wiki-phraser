#include <iostream>
#include <filesystem>
#include <fstream>

#include <vector>

namespace fs = std::filesystem;

#include "string_helper.hpp"
#include "args.hpp"
#include "logger.hpp"

Args::Args (Logger* logger) {
    if (logger != nullptr) {
        Args::logger = logger;
    }
    for (bool& bits : this->flags) {
        bits = false;
    }
};

bool Args::get (const Flags& flag) {
    return this->flags[static_cast<size_t>(flag)];
}

Args Args::check_args (const int argc, const char** argv) {
    
    Args arguments(Args::logger);
    
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
                                        
                            exit(0);
                        
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
        exit(1);
    } else if (args.size() - i > 2) {
        std::cerr << "WARNIG: too many Aruments!\n\tignoring all after ./phraser [<Flags>] <Source> <Destionation>\n";
    }
    
    arguments.source = chomp(args[i]);
    arguments.dest = chomp(args[i+1]);
    
    Args::logger->update(arguments);
    
    *(Args::logger) << Logger::format_flag(arguments.flags[static_cast<size_t>(Flags::VERBOSE)], "verbose")
                    << Logger::format_flag(arguments.flags[static_cast<size_t>(Flags::SCRIPT)], "is script")
                    << Logger::format_flag(arguments.flags[static_cast<size_t>(Flags::OVERRIDE)], "override")
                    << Logger::format_flag(arguments.flags[static_cast<size_t>(Flags::CREATE_DUMMY)], "create dummy")
                    << Logger::format_flag(arguments.flags[static_cast<size_t>(Flags::LOGFILE)], "file logging", arguments.logfile)
                    << Logger::format_flag(arguments.flags[static_cast<size_t>(Flags::UPDATE)], "update", arguments.update_script)
                    << "source:      " << arguments.source << "\n"
                    << "destination: " << arguments.dest << "\n";
    
    if (!arguments.get(Flags::SCRIPT)) {
        *(Args::logger) << "continue? (cancel with STRG+C) ";
        std::cin.ignore();
    }
    
    return arguments;
}
