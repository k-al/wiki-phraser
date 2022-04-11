
#ifndef STRINGHELPER_GUARD
#define STRINGHELPER_GUARD

#include <string>
#include <filesystem>

std::string chomp (const std::string& in);

std::string read_file (std::ifstream& file);
std::string read_file (const std::filesystem::path& file);

std::vector<std::string> split(const std::string& s, char splitChar);

size_t match_brackets (const std::string& string, size_t start_bracket);

bool is_at (const std::string& string, size_t pos, const std::string& match);

std::string to_lower (const std::string& string);

namespace strhelp {
    static const std::string word_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
}

#endif
