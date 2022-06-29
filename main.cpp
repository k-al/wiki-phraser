#include <iostream>
#include <filesystem>
#include <fstream>

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "string_helper.hpp"
#include "logger.hpp"
#include "args.hpp"


#include "phraser.hpp"



int main (const int argc, const char** argv) {
    
    Logger logger;
    Args arguments(&logger);
    
    arguments = Args::check_args(argc, argv);
    
    set_globals(logger, arguments);
    process_meta();
    process_command();
}
