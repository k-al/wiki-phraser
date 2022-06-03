
#ifndef STRINGHELPER_GUARD
#define STRINGHELPER_GUARD

#include <string>
#include <filesystem>

struct StrRange {
    std::string* base;
    size_t start = 0;
    size_t length = 0;
    
    StrRange (std::string& base, size_t start, size_t length);
    StrRange (StrRange& base, size_t start, size_t length);
    StrRange (std::string& base);
    
    char operator[] (const size_t pos);
    char at (const size_t pos);
    
    size_t end ();
    bool contains_index_of_base (size_t pos);
    bool conatins_index (size_t pos);
    
    size_t find_first_of (char match, size_t pos = 0);
    size_t find_first_of (std::string match, size_t pos = 0);
    size_t find_first_not_of (char match, size_t pos = 0);
    size_t find_first_not_of (std::string match, size_t pos = 0);
    size_t find_last_of (char match, size_t pos = std::string::npos);
    size_t find_last_of (std::string match, size_t pos = std::string::npos);
    size_t find_last_not_of (char match, size_t pos = std::string::npos);
    size_t find_last_not_of (std::string match, size_t pos = std::string::npos);
    
    std::string get ();
}


std::string chomp (const std::string& in);

std::string read_file (std::ifstream& file);
std::string read_file (const std::filesystem::path& file);

std::vector<std::string> split(const std::string& s, char splitChar);

size_t match_brackets (const std::string& string, size_t start_bracket);

bool is_at (const std::string& string, size_t pos, const std::string& match);
bool is_at (const StrRange& string, size_t pos, const std::string& match);

std::string to_lower (const std::string& string);

namespace strhelp {
    static const std::string word_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
}

#endif
