#include <iostream>
#include <filesystem>
#include <fstream>

#include <vector>

namespace fs = std::filesystem;

#include "string_helper.hpp"
#include "args.hpp"
#include "logger.hpp"


Args Args::get = Args();

Args::Args () {
    for (bool& el : this->flags) {
        el = false;
    }
}

bool Args::flag (const Flags& flag) {
    return this->flags[static_cast<size_t>(flag)];
}

void print_help () {
    std::cout << "./phraser [<FLAGS>] [-] <source> <destination>\n\n"
              << "Flags:\n"
              << "-v\n\tverbose mode\n\n"
              << "-h\n\tshow this help\n\n";

    exit(0);
}

/*
 * try to find the specified file and throws if it can't access them
 * if @param{input} = false it tries to create the file in the specified path
 */
void check_file (fs::path path, bool input = true) {
    fs::directory_entry file(path);


    if (input) {
        try {
            std::ifstream stream(path, std::ios_base::in);
            stream.close();
        } catch (std::system_error e) {
            throw "can't read the File >" + path.string() + "< \n\t" + e.what();
        }
    } else {
        try {
            std::ofstream stream (path, std::ios_base::app);
            stream.close();
        } catch (std::system_error e) {
            throw "can't write the File >" + path.string() + "< \n\t" + e.what();
        }
    }
}

void Args::check_args (const int argc, const char** argv) {
    
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
                        Args::get.flags[static_cast<size_t>(Flags::UPDATE)] = true;
                        Args::get.update_script = chomp(args[i]);
                    } else {
                        std::cerr << "WARNIG: word-flag '--update' was used, but no update script was specified!\n  continuing by ignoring the flag.\n";
                    }
                } else if (flag == "log") {
                    if ((i+1) < args.size() && args[i+1][0] != '-') {
                        i++;
                        Args::get.flags[static_cast<size_t>(Flags::LOGFILE)] = true;
                        Args::get.logfile = chomp(args[i]);
                    } else {
                        std::cerr << "WARNIG: word-flag '--log' was used, but no log file was specified!\n  continuing by ignoring the flag.\n";
                    }
                } else if (flag == "log") {
                    if ((i+1) < args.size() && args[i+1][0] != '-') {
                        i++;
                        Args::get.flags[static_cast<size_t>(Flags::LOGFILE)] = true;
                        Args::get.logfile = chomp(args[i]);
                    } else {
                        std::cerr << "WARNIG: word-flag '--log' was used, but no log file was specified!\n  continuing by ignoring the flag.\n";
                    }
                }
            } else { // the -f flags
                for (int j = 1; j < akt_arg->length(); j++) {
                    switch ((*akt_arg)[j]) {
                        case 'v':
                            Args::get.flags[static_cast<size_t>(Flags::VERBOSE)] = true;
                            break;
                        
                        case 'o':
                            Args::get.flags[static_cast<size_t>(Flags::OVERRIDE)] = true;
                            break;
                        
                        case 'd':
                            Args::get.flags[static_cast<size_t>(Flags::CREATE_DUMMY)] = true;
                            break;
                        
                        case 'u':
                            Args::get.flags[static_cast<size_t>(Flags::UPDATE)] = true;
                            Args::get.update_script = "./update.sh";
                            break;
                        
                        case 's':
                            Args::get.flags[static_cast<size_t>(Flags::SCRIPT)] = true;
                            break;
                        
                        case 'l':
                            Args::get.flags[static_cast<size_t>(Flags::LOGFILE)] = true;
                            Args::get.logfile = "./phraser.log";
                            break;
                        
                        case 'h':
                            print_help();
                        
                        default:
                            std::cerr << "WARNIG: invalide single-char flag '-" << (*akt_arg)[j] << "' is ignored!\n";
                    }
                }
            }
        } else {
            break;
        }
    }
    
    //! standatize path-format (to absolute paths i guess) and check for validity (existing and permissions)
    
    if (args.size() - i < 2) {
        std::cerr << "ERROR: too few Argumets!\n\tneeded: ./phraser [<Flags>] <Source> <Destionation>\n";
        exit(1);
    } else if (args.size() - i > 2) {
        std::cerr << "WARNIG: too many Aruments!\n\tignoring all after ./phraser [<Flags>] <Source> <Destionation>\n";
    }
    
    Args::get.source = chomp(args[i]);
    Args::get.dest = chomp(args[i+1]);
    
    Logger::out << Logger::format_flag(Args::get.flags[static_cast<size_t>(Flags::VERBOSE)], "verbose")
                    << Logger::format_flag(Args::get.flags[static_cast<size_t>(Flags::SCRIPT)], "is script")
                    << Logger::format_flag(Args::get.flags[static_cast<size_t>(Flags::OVERRIDE)], "override")
                    << Logger::format_flag(Args::get.flags[static_cast<size_t>(Flags::CREATE_DUMMY)], "create dummy")
                    << Logger::format_flag(Args::get.flags[static_cast<size_t>(Flags::LOGFILE)], "file logging", Args::get.logfile)
                    << Logger::format_flag(Args::get.flags[static_cast<size_t>(Flags::UPDATE)], "update", Args::get.update_script)
                    << "source:      " << Args::get.source << "\n"
                    << "destination: " << Args::get.dest << "\n";
    
    if (!Args::get.flag(Flags::SCRIPT)) {
        Logger::out << "continue? (cancel with STRG+C) ";
        std::cin.ignore();
    }
}
