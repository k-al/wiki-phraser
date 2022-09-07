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
        Section,

        number
    };

    StrRange square_para;
    StrRange curly_para;
    CommandType type;

    static std::pair<StrRange, StrRange> get_brackets (StrRange& base, size_t pos);

    Command ();
    Command (StrRange& base, size_t pos);
    void clear ();
};

struct HtmlPath;

struct HtmlPathParentIterator {
    HtmlPath* path = nullptr;
    size_t number = 0;
    std::string ret;
    
    HtmlPathParentIterator();
    HtmlPathParentIterator(HtmlPath* path);
    
    HtmlPathParentIterator& operator++ ();
    bool operator!= (const HtmlPathParentIterator& other);
    std::string operator* ();
};

struct HtmlPath {
    HtmlPath* relative_to = nullptr;
    std::vector<std::string> elements;
    
    HtmlPath ();
    HtmlPath (fs::path fs_path, HtmlPath* offset = nullptr);
    HtmlPath (std::string str_path, HtmlPath* offset = nullptr);
    
    std::string operator[] (size_t index);
    
    HtmlPathParentIterator begin();
    HtmlPathParentIterator end();
    
    void make_absolute ();
    HtmlPath path_to (HtmlPath dest);
    std::string get ();
};


std::string html_newline ();

#endif
