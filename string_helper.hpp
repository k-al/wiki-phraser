
#ifndef STRINGHELPER_GUARD
#define STRINGHELPER_GUARD

#include <string>

std::string chomp (const std::string& in);

std::string read_file (std::ifstream& file);

std::vector<std::string> split(const std::string& s, char splitChar);

#endif
