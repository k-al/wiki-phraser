
std::string chomp (const std::string& in) {
    bool ecased = false;
    size_t start = in.find_first_not_of(" \n\t");
    size_t end = in.find_last_not_of(" \n\t");
    end++;
    if (in[start] == '\'' || in[start] == '"') {
        size_t pair = in.find_last_of(in[start]);
        if (pair != start) {
            end = pair - 1;
            start++;
        }
    }
//     std::cout << "chomp: " << in << " (" << in.length() << ") (" << start << "|" << end << ")\n";  
    end -= start;
    
    if (end <= 0) {
        return "";
    } else {
        return in.substr(start, end);
    }
};

std::string print_flag (const bool& flag, const std::string& flag_name, const std::string& flag_parameter = "") {
    return static_cast<std::string>("[") + (flag ? "x" : " ") + "] " + flag_name + ((flag && flag_parameter != "") ? ("\t -> " + flag_parameter + "\n") : "\n");
};

class Logger {
public:
    Args* args;
    
    void update (Args& args) {
        this->args = &args;
    }
    
    bool to_file () const {
        if (this->args == nullptr) {
            std::cerr << "[Error in to_file-check]logger has invalide pointer to the argument struct!\n";
            throw 1;
        }
        return this->args->flags[static_cast<size_t>(Flags::LOGFILE)];
    }
    
    bool verbose () const {
        if (this->args == nullptr) {
            std::cerr << "[Error in verbose-check] Logger has invalide pointer to the argument struct!\n";
            throw 1;
        }
        return this->args->flags[static_cast<size_t>(Flags::VERBOSE)];
    }
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
