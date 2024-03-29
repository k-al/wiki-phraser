#include <vector>
#include <fstream>
#include <sstream>
#include "string_helper.hpp"

#include "logger.hpp"
static Logger& logger = Logger::out;


StrRange::StrRange () {
    this->base = nullptr;
    this->start = 0;
    this->length = 0;
}

StrRange::StrRange (const std::string& base, size_t start, size_t length = std::string::npos) {
//     Logger::out << "StrRange: got string of length " << std::to_string(base.length())
//         << " and construct a range from " << std::to_string(start)
//         << " with length of " << (length == std::string::npos ? "infinty" : std::to_string(length)) << "\n";
    
    if (length == std::string::npos) {
        length = base.length() - start;
    }
    
    if (start + length < start) {
        throw std::length_error("Range constructed out of range of base string (base + length overflow)");
    }
    
    if (base.size() < (start + length)) {
        throw std::length_error("Range constructed out of range of base string");
    }
    
    this->base = &base;
    this->start = start;
    this->length = length;
}

StrRange::StrRange (const StrRange& base, size_t start, size_t length = std::string::npos) {
//     Logger::out << "StrRange: got range of length " << std::to_string(base.length)
//         << " and construct a range from " << std::to_string(start)
//         << " with length of " << (length == std::string::npos ? "infinty" : std::to_string(length)) << "\n";
    
    if (length == std::string::npos) {
        length = base.length - start;
    }
    
    if (start + length < start) {
        throw std::length_error("Range constructed out of range of base string (base + length overflow)");
    }
    
    if (base.length < (start + length)) {
        throw std::length_error("Range constructed out of range of base string-range");
    }
    
    this->base = base.base;
    this->start = base.start + start;
    this->length = length;
}

StrRange::StrRange (const std::string& base) {
    this->base = &base;
    this->start = 0;
    this->length = base.length();
}

StrRange::StrRange (const StrRange& base) {
    this->base = base.base;
    this->start = base.start;
    this->length = base.length;
}

bool StrRange::operator== (const std::string equivalent) const {
    return this->length == equivalent.length() && is_at(*this, 0, equivalent);
}

char StrRange::operator[] (const size_t pos) const {
    return (*this->base)[this->start + pos];
}

char StrRange::at (const size_t pos) const {
    if (pos >= this->length)
        throw std::runtime_error("Tried to acess on char out of Range");
    
    return (*this->base)[this->start + pos];
}

size_t StrRange::end () const {
    return this->start + this->length;
}

bool StrRange::contains_index_of_base (size_t pos) const {
    return this->start <= pos && pos < (this->start + this->length);
}

bool StrRange::contains_index (size_t pos) const {
    return pos < this->length;
}

size_t StrRange::find_first_of (char match, size_t pos) const {
    for (size_t i = pos; this->contains_index(pos); i++) {
        if ((*this)[i] == match)
            return i;
    }
    return std::string::npos;
}

size_t StrRange::find_first_of (std::string match, size_t pos) const {
    for (size_t i = pos; this->contains_index(pos); i++) {
        for (auto match_it = match.cbegin(); match_it != match.cend(); match_it++) {
//             Logger::out << "[" << std::to_string(i) << "]" << (*this)[i] + " <-> " + *match_it << ((*this)[i] == *match_it ? "true\n" : "false\n");
            if ((*this)[i] == *match_it)
                return i;
        }
    }
    return std::string::npos;
}

size_t StrRange::find_first_not_of (char match, size_t pos) const {
    for (size_t i = pos; this->contains_index(pos); i++) {
        if ((*this)[i] != match)
            return i;
    }
    return std::string::npos;
}

size_t StrRange::find_first_not_of (std::string match, size_t pos) const {
    for (size_t i = pos; this->contains_index(pos); i++) {
        for (auto match_it = match.cbegin(); match_it != match.cend(); match_it++) {
            if ((*this)[i] == *match_it)
                goto MATCH_FOUND; // break and skip return
        }
        return i; // return only if the whole for was running without a match
        MATCH_FOUND:;
    }
    return std::string::npos;
}

size_t StrRange::find_last_of (char match, size_t pos) const {
    if (pos > this->length)
        pos = this->length;
    
    for (size_t i = pos; i > 0; ) {
        i--;
        if ((*this)[i] == match)
            return i;
    }
    return std::string::npos;
}

size_t StrRange::find_last_of (std::string match, size_t pos) const {
    if (pos > this->length)
        pos = this->length;
    
    for (size_t i = pos; i > 0; ) {
        i--;
        for (auto match_it = match.cbegin(); match_it != match.cend(); match_it++) {
            if ((*this)[i] == *match_it)
                return i;
        }
    }
    return std::string::npos;
}

size_t StrRange::find_last_not_of (char match, size_t pos) const {
    if (pos > this->length)
        pos = this->length;
    
    for (size_t i = pos; i > 0; ) {
        i--;
        if ((*this)[i] != match)
            return i;
    }
    return std::string::npos;
}

size_t StrRange::find_last_not_of (std::string match, size_t pos) const {
    if (pos > this->length)
        pos = this->length;
    
    for (size_t i = pos; i > 0; ) {
        i--;
        for (auto match_it = match.cbegin(); match_it != match.cend(); match_it++) {
            if ((*this)[i] == *match_it)
                goto MATCH_FOUND; // break and skip return
        }
        return i; // return only if the whole for was running without a match
        MATCH_FOUND:;
    }
    return std::string::npos;
}

std::string StrRange::get () const {
    return this->base->substr(this->start, this->length);
}


/**
 * chops off the first @param pos characters and returns them in a new StrRange\n
 * if pos is greater than the length of the calling StrRange it returns the whole StrRange and invalidates the calling StrRange (set length to 0)
 * @param pos position of the cut (cuts BEFORE the n'th character)
 * @return StrRange of the cutted characters
 */
StrRange StrRange::consume_to (size_t pos) {
    StrRange res;
    if (!this->contains_index(pos)) {
        res = StrRange(*this);
        
        this->length = 0;
        
        return res;
    } else {
        res = StrRange(*this, 0, pos);
        
        this->length -= pos;
        this->start += pos;
        
        return res;
    }
}


/**
 * chops off everything after the first @param pos characters and returns it in a new StrRange\n
 * if pos is greater than the length of the calling StrRange it returns an empty StrRange and does not modify the calling StrRange
 * @param pos position of the cut (cuts AFTER the n'th character)
 * @return StrRange of the cutted characters
 */
StrRange StrRange::consume_from (size_t pos) {
    if (this->contains_index(pos)) {
        StrRange res = StrRange(*this, pos);
        this->length = pos;
        return res;
    }
    return StrRange(*this, pos, 0);
}

std::string chomp (const std::string& in) {
    const size_t start = in.find_first_not_of(strhelp::white_chars);
    if (start == std::string::npos) {
        return "";
    }
    
    const size_t end = in.find_last_not_of(strhelp::white_chars) + 1;
    // checking not necessary, because previous check showed that there is a non-whitespace char in 'in'
    
    return in.substr(start, end - start);
};

void chomp (StrRange& in) {
    const size_t start = in.find_first_not_of(strhelp::white_chars);
    if (start == std::string::npos) {
        in.consume_from(0);
        return;
    }
    
    const size_t end = in.find_last_not_of(strhelp::white_chars) + 1;
    // checking not necessary, because previous check showed that there is a non-whitespace char in 'in'
    
    in.consume_to(start);
    in.consume_from(end);
    return;
}

/**
 * reads a file into a string\n
 * every character of the file will be copied
 * @param file the file to read from
 * @return the file content as string
 */
std::string read_file (std::ifstream& file) {
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    std::string buffer(size, '\0');
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

    const std::string brackets = {end_b, start_b};
    size_t pos = string.find_first_of(brackets, start_bracket + 1);

    while (string[pos] == start_b) {
        pos = match_brackets(string, pos);
        
        if (pos == std::string::npos)
            return std::string::npos;
        
        pos = string.find_first_of(brackets, pos + 1);
        
        if (pos == std::string::npos)
            return std::string::npos;
    }
    
    return pos;
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

    const std::string brackets = {end_b, start_b};
    size_t pos = string.find_first_of(brackets, start_bracket + 1);

    while (string[pos] == start_b) {
        pos = match_brackets(string, pos);
        
        if (pos == std::string::npos)
            return std::string::npos;
        
        pos = string.find_first_of(brackets, pos + 1);
        
        if (pos == std::string::npos)
            return std::string::npos;
    }
    
    return pos;
}

/**
 *
 * @param string string that is searched
 * @param pos position where the match should be found
 * @param match string that should be found at the position
 * @return true if and only if @param string at position @param pos reads @param match
 */
bool is_at (const std::string& string, size_t pos, const std::string& match) {
logger < "got into is_at(" < string < " | " < match < ") at " < pos < "\n";
    if ((pos + match.size()) > string.size())
        return false;
    
// logger < "match size " < match.size() < "\n";
    for (size_t i = 0; i < match.size(); i++) {
// logger < "number " < i < "\n";
// logger < " m:" < match[i] < " s:" < string[pos + i] < "\n";
        if (match[i] != string[pos + i])
            return false;
    }
logger < "ready\n";
    return true;
}

bool is_at (const StrRange& string, size_t pos, const std::string& match) {
    if (!string.contains_index(pos + match.size()))
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

