
#ifndef STRINGHELPER_GUARD
#define STRINGHELPER_GUARD

#include <string>
#include <filesystem>

struct StrRange {
    std::string* base;
    size_t start = 0;
    size_t length = 0;
    
    StrRange (std::string& base, size_t start, size_t length);
    
    char StrRange::operator[] (size_t pos);
    
    size_t end ();
    bool contains_index_of_base (size_t pos);
    bool conatins_index (size_t pos);
    
    std::string get ();
}


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
