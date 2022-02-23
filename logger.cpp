#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;



#include "logger.hpp"

#include "args.hpp"


void Logger::update (Args& args) {
    this->args = &args;
}

bool Logger::to_file () const {
    if (this->args == nullptr) {
        std::cerr << "[Error in to_file-check]logger has invalide pointer to the argument struct!\n";
        throw 1;
    }
    return this->args->flags[static_cast<size_t>(Flags::LOGFILE)];
}

bool Logger::verbose () const {
    if (this->args == nullptr) {
        std::cerr << "[Error in verbose-check] Logger has invalide pointer to the argument struct!\n";
        throw 1;
    }
    return this->args->flags[static_cast<size_t>(Flags::VERBOSE)];
}

std::string Logger::format_flag (const bool& flag, const std::string& flag_name, const std::string& flag_parameter) {
    return static_cast<std::string>("[") + (flag ? "x" : " ") + "] " + flag_name + ((flag && flag_parameter != "") ? ("\t -> " + flag_parameter + "\n") : "\n");
};


Logger& operator<< (Logger& logger, const std::string& log) {
    if (logger.to_file()) {
        //! get file-handle from Logger and start writing
    } else {
        std::cout << log;
    }
    return logger;
}

Logger operator< (Logger& logger, const std::string& log) {
    if (logger.verbose()) {
        logger << log;
    }
    return logger;
}
