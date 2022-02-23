

#ifndef ARGS_GUARD
#define ARGS_GUARD


#include <array>

enum class Flags {
    VERBOSE = 0, // write out exactly what is going on
    LOGFILE, // dont print to STDOUT and use an specified logfile instead
    OVERRIDE, // rewrite everything at destination (formaly: delete everything in destination before continuing)
    CREATE_DUMMY, // create dummyfiles if a link goes into nowhere
    UPDATE, // update dierectly (update script must be at ./update.sh OR .\update.bat) (more spcification in the implementation)
    SCRIPT, // don't ask questions
    number
};

class Logger;

struct Args {
    inline static Logger* logger;
    
    std::array<bool, static_cast<size_t>(Flags::number)> flags;
    fs::path logfile;
    fs::path update_script;
    fs::path source;
    fs::path dest;
    
    Args (Logger* logger);
    
    bool get (const Flags& flag);
    
    static Args check_args (const int argc, const char** argv);
};

#endif
