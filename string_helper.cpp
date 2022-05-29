#include <vector>
#include <fstream>
#include <sstream>
#include "string_helper.hpp"


StrRange::StrRange (const std::string& base, size_t start, size_t length) {
    
    //! integer overflow vunerable
    if (base.size() < (start + length))
        throw std::lenth_error("Range constructed out of range of base string");
    
    
    this->base = &base;
    this->start = start;
    this->length = length;
}

StrRange::StrRange (const StrRange& base, size_t start, size_t length = std::string::npos) {
    if (length == std::string::npos) {
        length = base.length - start;
        
    //! integer overflow vunerable
    } else if (base.conatins_index(start + length)) {
        throw std::lenth_error("Range constructed out of range of base string-range");
    }
    
    this->base = base->base;
    this->start = base->start + start;
    this->length = length
}

char StrRange::operator[] (const size_t pos) {
    return this->base->[this->start + pos];
}

char StrRange::at (const size_t pos) {
    if (pos >= this->length)
        throw std::runtime_error("Tried to acess on char out of Range");
    
    return this->base->[this->start + pos];
}

size_t StrRange::end () {
    return this->start + this->length;
}

bool StrRange::conatins_index_of_base (size_t pos) {
    return this->start <= pos && pos < (this->start + this->length);
}

bool StrRange::conatins_index (size_t pos) {
    return pos < this->length;
}

std::string StrRange::get () {
    return this->base->substr(this->start, this->length);
}

std::string chomp (const std::string& in) {
    size_t start = in.find_first_not_of(" \n\t");
    size_t end = in.find_last_not_of(" \n\t") + 1;
    
    //! this is dangerous and should be in another function
    if (in[start] == '\'' || in[start] == '"') {
        size_t pair = in.find_last_of(in[start]);
        if (pair != start) {
            end = pair - 1;
            start++;
        }
    }
    
//     std::cout << "chomp: " << in << " (" << in.length() << ") (" << start << "|" << end << ")\n";  
    end -= start;
    
    if (end <= 0) {
        return "";
    } else {
        return in.substr(start, end);
    }
};


/**
 * reads a file into a string\n
 * every character of the file will be copied
 * @param file the file to read from
 * @return the file content as string
 */
std::string read_file (std::ifstream& file) {
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    std::string buffer(size, ' ');
    file.seekg(0);
    file.read(&buffer[0], size);
    return buffer;
};

std::string read_file (const std::filesystem::path& file) {
    std::ifstream in_file(file.string());
    return read_file(in_file);
};

/**
 * Splits a string in substring at the given splitChar
 * @param s the string to split
 * @param splitChar the char to split at
 * @return a vector of string containing the substrings
 */
std::vector<std::string> split(const std::string& s, char splitChar) {
    std::vector<std::string> subsets;
    std::string subset;
    std::istringstream subsetStream(s);
    
    while (std::getline(subsetStream, subset, splitChar)) {
        subsets.push_back(chomp(subset));
    }
    
    return subsets;
}

/**
 *
 * @param string
 * @param start_bracket position of the starting bracket
 *                      acceptable chars at this position: {[(<
 * @return position of the closing bracket if present. std::string::npos otherwise
 */
size_t match_brackets (const std::string& string, size_t start_bracket) {
    char end_b;
    const char start_b = string.at(start_bracket);
    if (start_b == '{') {
        end_b = '}';
    } else if (start_b == '[') {
        end_b = ']';
    } else if (start_b == '(') {
        end_b = ')';
    } else if (start_b == '<') {
        end_b = '>';
    } else {
        return std::string::npos;
    }

    size_t pos = start_bracket;
    size_t open = string.find_first_of(start_b, pos + 1);
    size_t close = string.find_first_of(end_b, pos + 1);

    while (open < close) {
        pos = match_brackets(string, open);
        open = string.find_first_of(start_b, pos + 1);
        close = string.find_first_of(end_b, pos + 1);
    }
    
    return close;
}

size_t match_brackets (const StrRange& string, size_t start_bracket) {
    char end_b;
    const char start_b = string.at(start_bracket);
    if (start_b == '{') {
        end_b = '}';
    } else if (start_b == '[') {
        end_b = ']';
    } else if (start_b == '(') {
        end_b = ')';
    } else if (start_b == '<') {
        end_b = '>';
    } else {
        return std::string::npos;
    }

    size_t pos = start_bracket;
    size_t open = string.find_first_of(start_b, pos + 1);
    size_t close = string.find_first_of(end_b, pos + 1);

    while (open < close) {
        pos = match_brackets(string, open);
        open = string.find_first_of(start_b, pos + 1);
        close = string.find_first_of(end_b, pos + 1);
    }
    
    return close;
}

/**
 *
 * @param string string that is searched
 * @param pos position where the match should be found
 * @param match string that should be found at the position
 * @return true if and only if @param string at position @param pos reads @param match
 */
bool is_at (const std::string& string, size_t pos, const std::string& match) {
    if ((pos + match.size()) > string.size())
        return false;
    
    for (size_t i = 0; i < match.size(); i++) {
        if (match[i] != string[pos + i])
            return false;
    }
    return true;
}

bool is_at (const StrRange& string, size_t pos, const std::string& match) {
    if (!string.conatins_index(pos + match.size()))
        return false;
    
    for (size_t i = 0; i < match.size(); i++) {
        if (match[i] != string[pos + i])
            return false;
    }
    return true;
}

std::string to_lower (const std::string& string) {
    std::string res = string;
    for (size_t i = 0; i < res.size(); i++) {
        if (res[i] <= 'Z' && res[i] >= 'A')
            res[i] -= ('Z' - 'z');
    }
    return res;
}

