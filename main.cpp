#include <iostream>
#include <filesystem>
#include <fstream>

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace fs = std::filesystem;


#include "string_helper.hpp"
#include "logger.hpp"
#include "args.hpp"


#include "phraser.hpp"



int main (const int argc, const char** argv) {
    
    Logger logger;
    Args arguments(&logger);
    
    arguments = Args::check_args(argc, argv);
    
    Phraser::phraser_main(arguments, logger);
}
