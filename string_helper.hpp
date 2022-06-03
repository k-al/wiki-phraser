
#ifndef STRINGHELPER_GUARD
#define STRINGHELPER_GUARD

#include <string>
#include <filesystem>

struct StrRange {
    const std::string* base;
    size_t start = 0;
    size_t length = 0;
    
    StrRange ();
    StrRange (const std::string& base, size_t start, size_t length);
    StrRange (const StrRange& base, size_t start, size_t length);
    StrRange (const std::string& base);
    
    char operator[] (const size_t pos) const;
    char at (const size_t pos) const;
    
    size_t end () const;
    bool contains_index_of_base (size_t pos) const;
    bool contains_index (size_t pos) const;
    
    size_t find_first_of (char match, size_t pos = 0) const;
    size_t find_first_of (std::string match, size_t pos = 0) const;
    size_t find_first_not_of (char match, size_t pos = 0) const;
    size_t find_first_not_of (std::string match, size_t pos = 0) const;
    size_t find_last_of (char match, size_t pos = std::string::npos) const;
    size_t find_last_of (std::string match, size_t pos = std::string::npos) const;
    size_t find_last_not_of (char match, size_t pos = std::string::npos) const;
    size_t find_last_not_of (std::string match, size_t pos = std::string::npos) const;
    
    std::string get () const;
    
    void consume_to (size_t pos);
    void consume_from (size_t pos);
};


std::string chomp (const std::string& in);

std::string read_file (std::ifstream& file);
std::string read_file (const std::filesystem::path& file);

std::vector<std::string> split(const std::string& s, char splitChar);

size_t match_brackets (const std::string& string, size_t start_bracket);
size_t match_brackets (const StrRange& string, size_t start_bracket);

bool is_at (const std::string& string, size_t pos, const std::string& match);
bool is_at (const StrRange& string, size_t pos, const std::string& match);

std::string to_lower (const std::string& string);

namespace strhelp {
    static const std::string word_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
    static const std::string white_chars = " \t\n";
}

#endif
