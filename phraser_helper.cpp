

#include "phraser_helper.hpp"

Entry::Entry () = default;
Entry::Entry (Entry&) = default;
Entry::Entry (Entry&&) = default;


void Entry::set_content (std::string& content, Block block) {
    if (block != Block::number) {
        this->content[static_cast<size_t>(block)].swap(content);
    } else {
        throw std::runtime_error("Error: Tried to write content to non content block");
    }
}


std::pair<StrRange, StrRange> Command::get_brackets (StrRange& base, size_t pos) {
    StrRange square;
    StrRange curly;
    
    size_t bracket_pos = base.find_first_not_of(strhelp::white_chars, pos);
    
    if (is_at(base, bracket_pos, "[")) {
        size_t start = bracket_pos + 1;
        pos = match_brackets(base, bracket_pos);
        
        if (pos == std::string::npos)
            throw std::runtime_error("Not matching square bracket");
        
        square = StrRange(base, start, pos - start);
        chomp(square);
        
        pos++;
        bracket_pos = base.find_first_not_of(strhelp::white_chars, pos);
    }
    
    if (is_at(base, bracket_pos, "{")) {
        size_t start = bracket_pos + 1;
        pos = match_brackets(base, bracket_pos);
        
        if (pos == std::string::npos)
            throw std::runtime_error("Not matching curly bracket");
        
        curly = StrRange(base, start, pos - start);
        chomp(curly);
        
        pos++;
    }
    
    base.consume_to(pos);
    return std::pair<StrRange, StrRange>(square, curly);
}


Command::Command (StrRange& base, size_t pos) {
    std::pair<StrRange, StrRange> para;
    if (!base.contains_index(pos))
        throw std::runtime_error("Command starting point is out of given range");
    
    if (base[0] != '$')
        throw std::runtime_error("Command doesn't start with '$'");
    
    if (is_at(base, 1, "start")) {
        try {
            para = get_brackets(base, 6);
        } catch (std::runtime_error& e) {
            throw std::runtime_error(e.what() + static_cast<std::string>(" in $start command"));
        }
        this->type = CommandType::Start;
        
    } else if (is_at(base, 1, "linknames")) {
        try {
            para = get_brackets(base, 10);
        } catch (std::runtime_error& e) {
            throw std::runtime_error(e.what() + static_cast<std::string>(" in $linknames command"));
        }
        this->type = CommandType::Linknames;
        
    } else if (is_at(base, 1, "link")) {
        try {
            para = get_brackets(base, 5);
        } catch (std::runtime_error& e) {
            throw std::runtime_error(e.what() + static_cast<std::string>(" in $link command"));
        }
        this->type = CommandType::Link;
        
    } else if (is_at(base, 1, "time")) {
        try {
            para = get_brackets(base, 5);
        } catch (std::runtime_error& e) {
            throw std::runtime_error(e.what() + static_cast<std::string>(" in $time command"));
        }
        this->type = CommandType::Time;
        
    } else {
        StrRange command(base, 0, base.find_first_not_of(strhelp::word_chars, 1));
        throw std::runtime_error("Unknown command '" + command.get() + "'");
    }
}


void Command::clear () {
    this->square_para.length = 0; // invalidating the Ranges
    this->curly_para.length = 0;
    this->type = CommandType::number; // setting the type to NoType
}


std::string html_newline () {
    return "</br>\n"; // '\n' does nothing but to increase the readebility of the generated html file
}
