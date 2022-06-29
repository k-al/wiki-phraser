#ifndef PHRASER_HELPER_GUARD
#define PHRASER_HELPER_GUARD

#include <unordered_map>
#include <array>
#include <string>
#include <vector>
#include <filesystem>

#include "string_helper.hpp"

namespace fs = std::filesystem;

struct Entry {
    enum class Block {
        Main = 0,
        Side,
        
        number
    };
    
//     static std::unordered_multimap<std::string, Entry*> linkNames;

    bool phrased = false;
    bool directory = false;
    fs::path source;
    fs::path destination;
//     std::unordered_set<std::string> links; // set of paths that Entry links to
    std::unordered_map<std::string, std::vector<std::string>> tags;
    std::array<std::string, static_cast<size_t>(Block::number)> content;
    
    Entry ();
    Entry (Entry& lhs);
    Entry (Entry&& rhs);
    
    void set_content (std::string& content, Block block);
};


struct Command {
    enum class CommandType {
        Start = 0,
        Linknames,
        Link,
        Time,
        Listall,
        List,
        Tag,
        
        number
    };
    
    StrRange square_para;
    StrRange curly_para;
    CommandType type;
    
    static std::pair<StrRange, StrRange> get_brackets (StrRange& base, size_t pos);
    
    Command (StrRange& base, size_t pos);
    void clear ();
};


std::string html_newline ();

#endif
