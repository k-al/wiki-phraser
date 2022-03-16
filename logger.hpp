
#ifndef LOGGER_GUARD
#define LOGGER_GUARD


#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"
#define GREEN "\033[1;32m"
#define RESET "\033[0m"

#include <string>

struct Args;
enum class Flags;

class Logger {
public:
    Args* args;
    
    void update (Args& args);
    
    bool to_file () const;
    bool verbose () const;
    
    void print_flag (const Flags& flag) const;
    
    static std::string format_flag (const bool& flag, const std::string& flag_name, const std::string& flag_parameter = "");
};

Logger& operator<< (Logger& logger, const std::string& log);

Logger& operator< (Logger& logger, const std::string& log);

#endif
