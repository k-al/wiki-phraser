#include <vector>
#include <fstream>
#include <sstream>
#include "string_helper.hpp"

std::string chomp (const std::string& in) {
    size_t start = in.find_first_not_of(" \n\t");
    size_t end = in.find_last_not_of(" \n\t");
    end++;
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
 * @return the file content
 */
std::string read_file (std::ifstream& file) {
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    std::string buffer(size, ' ');
    file.seekg(0);
    file.read(&buffer[0], size);
    return buffer;
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