#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

#include "logger.hpp"

#include "args.hpp"


Logger Logger::out = Logger();

Logger::Logger () {};

bool Logger::to_file () {
    return Args::get.flags[static_cast<size_t>(Flags::LOGFILE)];
}

bool Logger::verbose () {
    return Args::get.flags[static_cast<size_t>(Flags::VERBOSE)];
}

std::string Logger::format_flag (const bool& flag, const std::string& flag_name, const std::string& flag_parameter) {
    return static_cast<std::string>("[") + (flag ? "x" : " ") + "] " + flag_name + ((flag && flag_parameter != "") ? ("\t -> " + flag_parameter + "\n") : "\n");
};

Logger& operator<< (Logger& logger, const std::string& log) {
    if (Logger::to_file()) {
        //! get file-handle from Logger and start writing
        throw std::logic_error("logging to file not yet implemented!");
    } else {
        std::cout << log;
    }
    return logger;
}

Logger& operator<< (Logger& logger, const long long& log) {
    return logger << std::to_string(log);
}

Logger& operator<< (Logger& logger, const long& log) {
    return logger << std::to_string(log);
}

Logger& operator<< (Logger& logger, const int& log) {
    return logger << std::to_string(log);
}

Logger& operator<< (Logger& logger, const short& log) {
    return logger << std::to_string(log);
}

Logger& operator<< (Logger& logger, const unsigned long long& log) {
    return logger << std::to_string(log);
}

Logger& operator<< (Logger& logger, const unsigned long& log) {
    return logger << std::to_string(log);
}

Logger& operator<< (Logger& logger, const unsigned int& log) {
    return logger << std::to_string(log);
}

Logger& operator<< (Logger& logger, const unsigned short& log) {
    return logger << std::to_string(log);
}

Logger& operator< (Logger& logger, const std::string& log) {
    if (logger.verbose()) {
        logger << log;
    }
    return logger;
}

Logger& operator< (Logger& logger, const long long& log) {
    return logger < std::to_string(log);
}

Logger& operator< (Logger& logger, const long& log) {
    return logger < std::to_string(log);
}

Logger& operator< (Logger& logger, const int& log) {
    return logger < std::to_string(log);
}

Logger& operator< (Logger& logger, const short& log) {
    return logger < std::to_string(log);
}

Logger& operator< (Logger& logger, const unsigned long long& log) {
    return logger < std::to_string(log);
}

Logger& operator< (Logger& logger, const unsigned long& log) {
    return logger < std::to_string(log);
}

Logger& operator< (Logger& logger, const unsigned int& log) {
    return logger < std::to_string(log);
}

Logger& operator< (Logger& logger, const unsigned short& log) {
    return logger < std::to_string(log);
}

bool operator&& (Logger& logger, const bool& operant) {
    return operant;
}
