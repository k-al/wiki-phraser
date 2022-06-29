




namespace Phraser {

    std::string image (fs::path path) {
        return "<div class=\"spacer\"><img class=\"Picture\" src=\"" + path.string() + "\"></div>";
    }

    std::string dropdownTitle (fs::path path, std::string name) {
        return "<div><a class=\"navtitle\" href=\"" + path.string() + "\" target=\"content\"><div>" + name + "</div></a></div>";
    }

    std::string dropdownLink (fs::path path, std::string name) {
        return "<a href=\"" + path.string() + "\" target=\"content\">\n"
               "     <div class=\"linkitem\">" + name + "</div>\n"
               "</a>";
    }



    static Args arguments;
    static Logger logger;
    
    struct Entry {
        enum class Block {
            Main = 0,
            Side,
            number
        };
        
        Entry () {};
        
        Entry (Entry& lhs) {
            
        }

        Entry (Entry&& rhs) {}

        static std::unordered_multimap<std::string, Entry*> linkNames;


        bool phrased = false;
        bool directory = false;
        fs::path source;
        fs::path destination;
        std::unordered_set<std::string> links; // set of paths that Entry links to
        std::unordered_map<std::string, std::vector<std::string>> tags;
        std::array<std::string, static_cast<size_t>(Block::number)> content;

        void set_content (std::string& content, Block block) {
            if (block != Block::number) {
                this->content[static_cast<size_t>(block)].swap(content);
            } else {
                Phraser::logger << RED << "Error: Tried to write content to non content block\n" << RESET;
            }
        }
    };
    std::unordered_multimap<std::string, Entry*> Entry::linkNames;
    
    static std::unordered_map<std::string, Entry*> entries;
    
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
        
        static std::pair<StrRange, StrRange> get_brackets (StrRange& base, size_t pos) {
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
        
        Command (StrRange& base, size_t pos) {
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
        
        void clear () {
            this->square_para.length = 0; // invalidating the Ranges
            this->curly_para.length = 0;
            this->type = CommandType::number; // setting the type to NoType
        }
    };
    
    

    /**
     * Reads the input Files and places the tags in the given Entry\n
     * also places the command section in content at pos 0
     * @param file the file Entry to phrase
     */
    void process_meta_single (Entry* file) {
        // read baseHtmlFiles
        // std::ifstream baseLayout;
        // baseLayout.open(fs::path("baseHtmlFiles/baseLayout.html"));
        // needed at writing at destination:
        //
        // static std::ifstream informationLayout;
        // informationLayout.open(fs::path("baseHtmlFiles/informationLayout.html"));
        // std::ofstream informationFile;
        // informationFile.open(file->destination);
        
        
        logger < "\n\nopen and read " < file->source.string() < "\n";

        const std::string buff = read_file(file->source);

        logger < "get all meta-data\n";
        {
            size_t position = 0;

            size_t first_command;
            size_t first_tag;
            
            bool default_linknames = true;

            while (true) {
                first_command = buff.find_first_of("$", position);
                first_tag = buff.find_first_of(":", position);

                if (first_command == std::string::npos) {
                    logger << RED << "ERROR: no content section in " << file->source.string() << "\n" << RESET << "\tforgot $start command?\n";
                    //? instead of exiting, continue with next file
                    exit(1);
                } else if (first_tag < first_command) {
                    size_t line_start = buff.find_last_of("\n", first_tag);
                    line_start++; // dont need the newline

                    std::string tag = buff.substr(line_start, first_tag - line_start);
                    tag = chomp(tag);



                    size_t line_end = buff.find_first_of("\n", first_tag);
                    line_end = line_end == -1 ? buff.size() : line_end;
                    std::string data = buff.substr(first_tag + 1, line_end - first_tag - 1);
                    std::vector<std::string> values = split(data, ',');

                    file->tags.insert(std::pair<std::string, std::vector<std::string>>(tag, values));

                    if (tag == "linknames") {
                        for (std::string linkn : values) {
                            file->linkNames.insert(std::pair<std::string, Entry*>(to_lower(linkn), file));
                        }
                    }

                    position = first_tag + 1;
                } else {
                    if (is_at(buff, first_command + 1, "start")) {
                        file->content[0] = buff.substr(first_command);
                        break;
                    } else if (is_at(buff, first_command + 1, "no_default_linknames")) {
                        default_linknames = false;
                    } else {
                        logger << RED << "ERROR: incorrect command in meta-section in " << file->source.string() << ":" << RESET
                               << " >$" << buff.substr(first_command, buff.find_first_not_of(strhelp::word_chars, first_command) - first_command) << "<\n";
                        //? instead of exiting, continue with next file
                        exit(1);
                    }
                }
            }
            
            if (default_linknames) {
                if (file->source.stem() != "index") {
                    file->linkNames.insert(std::pair<std::string, Entry*>(to_lower(file->source.stem()), file));
                }
                try {
                    file->linkNames.insert(std::pair<std::string, Entry*>(to_lower(file->tags.at("title").at(0)), file));
                } catch (const std::out_of_range& oor) { /* ignored */}
            }
        }
        logger < "done with meta-data from this file\n";
        return;
    }
    
    
    /**
     * scans dirs for files to phrase images to copy...\n
     * executes process_meta_single on all phrasable files to build up the entry data structure
     */
    void process_meta () {
        for (const fs::directory_entry& dir_entry : std::filesystem::recursive_directory_iterator(arguments.source)) {
            fs::path rel_path = dir_entry.path().lexically_relative(arguments.source);
            fs::path dest_path = (arguments.dest / rel_path).lexically_normal();
            
            logger < "\n\nFile: " < dir_entry.path() < "\n  relative Path: " < rel_path.string() < "\n  constructed destination Path: " < dest_path.string() < "\n";
            
            
            if (dir_entry.is_directory()) {
                // check the directory in the destination folder and create it if absent
                
                if (fs::create_directory(dest_path)) {
                    logger < "Is directory and gets copied to " < dest_path < "\n";
                } else {
                    logger < "Is directory and is allready present as " < dest_path < "\n";
                }
                
                //! TODO: make Entry which is later linked to right index
                
                continue;
            }
            
            if (dir_entry.is_regular_file()) {
                Entry* new_file = new Entry();
                new_file->source = dir_entry.path();
                new_file->destination = dest_path;


                entries.insert(std::pair<fs::path, Entry*>(rel_path.string(), new_file));
                
                
                if (rel_path.extension() == ".wikiph") {
                    dest_path.replace_extension("html");
                    new_file->destination = dest_path; // update struct
                    new_file->phrased = true;
                    logger < "Is a phrasable file and will be phrased to " < dest_path < "\n";
                    
                    process_meta_single(new_file);
                    
                } else {
                    logger < "is '" < rel_path.extension() < "' file and will be copied to " < dest_path < "\n";
                    if (rel_path.extension() == ".txt") {
                        logger < YELLOW < "WARNING: Do you really want a .txt on your website?\n" < RESET;
                    }

                    fs::directory_entry dest_entry(dest_path);
                    if (dest_entry.exists()) {
                        logger < "Destination file allready exists";
                        if (arguments.get(Flags::OVERRIDE)) {
                            logger < ", but overwrite it because of the '-o' Flag\n";
                            fs::copy_file(dir_entry.path(), dest_path, fs::copy_options::overwrite_existing);
                        } else {
                            fs::file_time_type dest_time = dest_entry.last_write_time();
                            fs::file_time_type source_time = dir_entry.last_write_time();
                            if (dest_time < source_time) {
                                logger < ", but overwrite it because it needs an update\n";
                                fs::copy_file(dir_entry.path(), dest_path, fs::copy_options::overwrite_existing);
                            } else {
                                logger < " and is up to date\n";
                            }
                        }
                    }  else {
                        fs::copy_file(dir_entry.path(), dest_path);
                    }
                }
                
            } else {
                logger << "WARNING: Can't handle " << dir_entry.path() << "\n \t Softlinks (for example) are not supported.\n";
            }
        }
    }
    
    
    
    std::string html_newline () {
        return "</br>\n"; // '\n' does nothing but to increase the readebility of the generated html file
    }
    
    void command_link (std::stringstream& out_stream, const Command& command, const Entry* active_entry) {
        
    }
    
    void command_time (std::stringstream& out_stream, const Command& command, const Entry* active_entry) {
        
    }
    
    
    void process_command () {
        for (std::pair<std::string, Entry*> it : entries) {
            // if not wikiph: skip
            if (!it.second->phrased) {
                continue;
            }
            StrRange work_string(it.second->content[0]);
            
            std::array<std::stringstream, static_cast<size_t>(Entry::Block::number)> content_builder;
            
            size_t pos;
            Entry::Block active_block;
            
            
            /**********************************
            * get the first '$start' command */
            
            pos = work_string.find_first_of('$');
            
            if (pos == std::string::npos) {
                //! TODO: extent the message with file information
                 throw std::runtime_error("Warning: No content found (forgot '$start' command?)\n");
            }
            
            Command command(work_string, pos);
            
            if (command.type != Command::CommandType::Start) {
                //! TODO: extent the message with file information
                throw std::runtime_error("Error: expected Start command (nr. "
                                        + std::to_string(static_cast<int>(Command::CommandType::Start))
                                        + "), but got nr. "
                                        + std::to_string(static_cast<int>(command.type))
                                        + "\n");
            }
            
            
            while (command.type != Command::CommandType::number) {
                
                /********************************
                * process the '$start' command */
                
                if (command.curly_para.length == 0
                    || is_at(command.curly_para, 0, "main"))
                {
                    active_block = Entry::Block::Main;
                    
                } else if (is_at(command.curly_para, 0, "side")) {
                    active_block = Entry::Block::Side;
                } else {
                    throw std::runtime_error("Error: unknown Block "
                                            + command.curly_para.get()
                                            + "specified in '$start' command\n");
                }
                
                /*******************************
                 * process all other commands */
                
                while (1) {
                    command.clear();
                    pos = work_string.find_first_of("\n$\\");
                    
                    if (pos = std::string::npos) {
                        content_builder[static_cast<size_t>(active_block)] << work_string.get();
                        break;
                    }
                    StrRange append = work_string.consume_to(pos); // does not get found char
                    content_builder[static_cast<size_t>(active_block)] << append.get();
                    
                    if (work_string[0] == '\n') {
                        
                        content_builder[static_cast<size_t>(active_block)] << html_newline();
                        work_string.consume_to(1); // remove the newline
                        
                    } else if (work_string[0] == '\\') {
                        
                        if (work_string.length == 1) {
                            //! TODO: extent the message with file information
                            logger << "Warning: dubious escape character '\\' at end of file gets ignored\n";
                            break;
                        }
                        
                        content_builder[static_cast<size_t>(active_block)] << work_string[1];
                        work_string.consume_to(2);
                        
                    } else if (work_string[0] == '$') {
                        
                        
                        try {
                            command = Command(work_string, 0);
                        } catch (const std::exception& e) {
                            //! TODO: extent the message with file information
                            throw;
                        }
                        
                        if (command.type == Command::CommandType::Start) {
                            break;
                        }
                        
                        switch (command.type) {
                            case Command::CommandType::Link:
                                command_link(content_builder[static_cast<size_t>(active_block)], command, it.second);
                                break;
                                
                            case Command::CommandType::Time:
                                command_time(content_builder[static_cast<size_t>(active_block)], command, it.second);
                        }
                        
                    } else {
                        //! TODO: extent the message with file information
                        throw std::runtime_error("Error: somethig went really wrong:\n\tsearched for '\\', '\\n' or '$' but found '"
                                            + std::string(1, work_string[0])
                                            + "'\n");
                    }
                }
            }
            return;
        }
    }
    
    
//     std::string process_command_text (Entry* file, const std::string& buff, size_t& pos, size_t end, size_t level) {
//         if (end == std::string::npos)
//             end = buff.size();
//         
//         if (pos >= end)
//             return "";
//         
//         std::string content = "";
//         
// 
//         while (true) {
//             { // get the new position
//                 size_t new_pos = end;
//                 
//                 size_t first_occ = buff.find_first_of('$', pos);
//                 new_pos = new_pos < first_occ ? new_pos : first_occ;
//                 
//                 first_occ = buff.find_first_of('\n', pos);
//                 new_pos = new_pos < first_occ ? new_pos : first_occ;
//                 
//                 if (pos != new_pos) {
//                     //? maybe code a string builder with vector as base for better memory management at growing strings
//                     content += buff.substr(pos, new_pos - pos);
//                 }
//                 
//                 pos = new_pos;
//             }
//             
//             if (pos == end) {
//                 return content;
//             }
//             
//             if (buff[pos] == '\n') { // replace all '\n' to "<\br>\n"
//                 content += "</br>\n";
//                 pos++;
//             } else if (buff[pos] == '$') { // found command
//                 
//                 // start command starts the next section, so return the gathered content
//                 if (is_at(buff, pos + 1, "start")) {
//                     if (level == 0) {
//                         return content;
//                     } else {
//                         logger << RED << "broken file " << file->source.string() << "!\n" << RESET << "\tgot $start command not on top level\n";
//                         exit(1);
//                     }
//                 }
//                 
//                 pos++;
//                 if (is_at(buff, pos, "link[")) {
//                     pos += 5;
//                     
//                     size_t link_start = pos;
//                     pos = match_brackets(buff, pos - 1);
//                     size_t link_ln = pos - link_start;
//                     
//                     fs::path link_path = get_path_from(buff, link_start, link_ln);
//                     
//                     if (pos == std::string::npos) {
//                         logger << RED << "missmatching backets in link command" << RESET;
//                         exit(1);
//                     }
//                     
//                     pos++;
//                     
//                     if (buff[pos] == '{') {
//                         // TODO
//                     }
//                 }
//                 
//             } else {
//                 logger << RED << "\nSomething went horribly wrong!\n was looking for '$' or '\\n', but got neither(" << ("" + buff[pos]) << ")\n" << RESET;
//                 exit(1);
//             }
//         }
//             
//     }
//     
//     void process_command () {
//         for (std::pair<std::string, Entry*> it : entries) {
//             // if not wikiph: skip
//             if (!it.second->phrased) {
//                 continue;
//             }
//             std::string buff;
//             buff.swap(it.second->content[0]);
//             
//             size_t pos = 0;
//             
//             while (pos < buff.size()) {
//                 if (!is_at(buff, pos, "$start")) {
//                     logger << RED << "broken file " << it.second->source.string() << "!\n" << RESET << "\texpectet >$start< but got >" << buff.substr(pos, 6) << "<\n";
//                     exit(1);
//                 }
//                 pos += 6;
//                 Entry::Block active_block;
//                 
//                 
//                 if (buff[pos] == ' ' || buff[pos] == '\n' || buff[pos] == '\t') {
//                     active_block = Entry::Block::Main;
//                 } else if (is_at(buff, pos, "{}")) {
//                     pos += 2;
//                     active_block = Entry::Block::Main;
//                 } else if (is_at(buff, pos, "{main}")) {
//                     pos += 6;
//                     active_block = Entry::Block::Main;
//                 } else if (is_at(buff, pos, "{side}")) {
//                     pos += 6;
//                     active_block = Entry::Block::Side;
//                 } else {
//                     logger << RED << "invalide $start command at " << it.second->source.string() << RESET << "\n";
//                     exit(1);
//                 }
//                 
//                 std::string content = process_command_text(it.second, buff, pos, buff.size(), 0);
//                 
//                 it.second->set_content(content, active_block);
//                 
//                 logger << it.first;
//             }
//         }
//     }

    // TODO: write content in einem stück
    std::string checkCommands(Entry* file, Entry::Block start, std::string buff, size_t open, size_t close, bool list) {
        size_t pos = open;
        std::string content = "";

        if (list) {
            content += "<ul>\n";
        }
        while (pos > close) {
            if(list) {
                content += "<li>\n";
            }
            size_t first_command = buff.find_first_of('$', pos);

            if (first_command == std::string::npos) {
                break;
            }
            // Build Html content
            std::string command = buff.substr(first_command + 1, buff.find_first_of('{', first_command) - first_command - 1);
            if(command == "start") {
                size_t open = buff.find_first_of('{', first_command);
                std::string block = buff.substr(open + 1, match_brackets(buff, open) - open - 1);
                file->set_content(content, start);
                start = block == "main" ? Entry::Block::Main : Entry::Block::Side;
            } else if (command == "time") {
                size_t open = buff.find_first_of('{', first_command);
                std::string time_str = buff.substr(open + 1, match_brackets(buff, open) - open - 1);
                std::string yer, mon, day;
                if (chomp(time_str).empty()) {
                    try {
                        yer = file->tags.at("time").at(0);
                        mon = file->tags.at("time").at(1);
                        day = file->tags.at("time").at(2);
                    } catch (const std::out_of_range& oor) {
                        logger << "failed to find time tag\n\t" << oor.what();
                    }
                } else {
                    yer = chomp(time_str.substr(open + 1, time_str.find_first_of(',') - 1));
                    mon = chomp(time_str.substr(time_str.find_first_of(',') + 1, time_str.find_last_of(',') - time_str.find_first_of(',') - 1));
                    day = chomp(time_str.substr(time_str.find_last_of(',') + 1));
                }
                content += "<h3>" + day == "00" ? "__" : day + "." + mon + "." + yer + "</h3><br>\n";
            } else if (command == "subsection") {
                size_t open = buff.find_first_of('[', first_command);
                size_t close = match_brackets(buff, open);
                std::string title = buff.substr(open + 1, close - open - 1);
                content += "<h2>" + title + "</h2><br>\n";
                open = buff.find_first_of('{', close);
                close = match_brackets(buff, open);
                std::string content = buff.substr(open + 1, close - open - 1);
                content += "<p>" + checkCommands(file, start, content, open, close, false) + "<p><br>\n";
            } else if (command == "list") {
                size_t open = buff.find_first_of('[', first_command);
                size_t close = match_brackets(buff, open);
                std::string title = buff.substr(open + 1, close - open - 1);
                content += "<h3>" + title + "</h3><br>\n";
                open = buff.find_first_of('{', close);
                close = match_brackets(buff, open);
                std::string content = buff.substr(open + 1, close - open - 1);
                content += "<p>" + checkCommands(file, start, content, open, close, true) + "<p><br>\n";
            } else if (command == "tags") {
                size_t open = buff.find_first_of('{', first_command);
                size_t close = match_brackets(buff, open);
                std::string title = buff.substr(open + 1, close - open - 1);
                content += "<ul>\n";
                for (std::string tag : file->tags.at(title)) {
                    content += "<li>" + tag + "</li>\n";
                }
                content += "</ul><br>\n";
            } else if (command == "linknames") {
                size_t open = buff.find_first_of('[', first_command);
                size_t close = match_brackets(buff, open);
                std::string links = buff.substr(open + 1, close - open - 1);

                size_t cmd = links.find_first_of('$');
                if(cmd == -1) {
                    int cmd = links.find_first_of(',');
                    if(cmd == -1) {
//                         if(file->linkNames.count(links) == 1) {
//                             content += "<a href=\"" + file->linkNames.at(links)->destination.string() + "\">" + links + "</a>";
//                         } else {
//                             // TODO: Error message
//                         }
                    } else {
                        for (std::string link : split(links, ',')) {
//                             if(file->linkNames.count(link) == 1) {
//                                 content += "<a href=\"" + file->linkNames.at(link)->destination.string() + "\">" + link + "</a>";
//                             } else {
//                                 // TODO: Error message
//                             }
                        }
                    }
                } else if (links.find("$tags") != std::string::npos) {
                    for (std::string link: file->tags.at(links.substr(links.find_first_of('{') + 1,
                                                                      links.find_last_of('}') -
                                                                      links.find_first_of('{') - 1))) {
//                         if (file->linkNames.count(link) == 1) {
//                             content += "<a href=\"" + file->linkNames.at(link)->destination.string() + "\">" + link + "</a>";
//                         } else {
//                             logger << YELLOW << "WARNING: Link to " << link << " not found" << RESET;
//                             content += link;
//                         }
                    }
                }
            } else {
                logger < "ERROR: Command not found!";
                exit(1);
            }
            pos = ++first_command;
            /* $subsection
             * $list
             * $start
             * $linknames
             * $time
             * $tags
             */
            if(list) {
                content += "</li>\n";
            }
        }
        if(list) {
            content += "</ul>\n";
        }
        return content;
    }

    
    
    void phraser_main (Args args, Logger log) {
        arguments = args;
        logger = log;
        
        //! this could be a problem:
        std::cout << "Problem: parent from " << (fs::current_path() / "..") << " is " << (fs::current_path() / "..").parent_path() << '\n';
        
        // read all files, build up the entry structure
        process_meta();
        
        // core phraser processing all the commands
        process_command();
        
        
    }
}
