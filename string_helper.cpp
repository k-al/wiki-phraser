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
