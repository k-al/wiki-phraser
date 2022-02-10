// #include <iostream>
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
        for (int i = 0; i < argc; i++) {
            std::string tmp(argv[i]);
            args.push_back(tmp);
        }
    }
    
//     sf::path::format::generic_format
    
//     for () {
//         
//     }
}
