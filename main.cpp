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
    
    Args::check_args(argc, argv);
    
    process_meta();
    Logger::out << "done with all meta-data\n";
    process_command();
}
