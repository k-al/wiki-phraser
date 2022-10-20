
#ifndef LOGGER_GUARD
#define LOGGER_GUARD


//! use operator overloading instead of defined constants
#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"
#define GREEN "\033[1;32m"
#define RESET "\033[0m"

#include <string>

struct Args;
enum class Flags;

class Logger {
private:
    Logger ();
public:
    static Logger out;
    
    static bool to_file ();
    static bool verbose ();
    
    static void print_flag (const Flags& flag);
    
    static std::string format_flag (const bool& flag, const std::string& flag_name, const std::string& flag_parameter = "");
    
    bool operator&& (const bool& operant) const;

    Logger (const Logger&) = delete;
    void operator=(const Logger&) = delete;
};

Logger& operator<< (Logger& logger, const std::string& log);
Logger& operator<< (Logger& logger, const long long& log);
Logger& operator<< (Logger& logger, const long& log);
Logger& operator<< (Logger& logger, const int& log);
Logger& operator<< (Logger& logger, const short& log);
Logger& operator<< (Logger& logger, const unsigned long long& log);
Logger& operator<< (Logger& logger, const unsigned long& log);
Logger& operator<< (Logger& logger, const unsigned int& log);
Logger& operator<< (Logger& logger, const unsigned short& log);

Logger& operator< (Logger& logger, const std::string& log);
Logger& operator< (Logger& logger, const long long& log);
Logger& operator< (Logger& logger, const long& log);
Logger& operator< (Logger& logger, const int& log);
Logger& operator< (Logger& logger, const short& log);
Logger& operator< (Logger& logger, const unsigned long long& log);
Logger& operator< (Logger& logger, const unsigned long& log);
Logger& operator< (Logger& logger, const unsigned int& log);
Logger& operator< (Logger& logger, const unsigned short& log);

bool operator&& (Logger& logger, const bool& operant);


#endif
