

#include "phraser.hpp"
#include "constants.hpp"

static Args& arguments = Args::get;
static Logger& logger = Logger::out;

static std::unordered_map<std::string, Entry*> entries;


void process_meta_single (Entry* file) {
    logger < "\n\nopen and read " < file->source.string() < "\n";
    const std::string buff = read_file(file->source);
    logger < "get all meta-data\n";
    
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
//                     file->linkNames.insert(std::pair<std::string, Entry*>(to_lower(linkn), file));
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
    
//     if (default_linknames) {
//         if (file->source.stem() != "index") {
//             file->linkNames.insert(std::pair<std::string, Entry*>(to_lower(file->source.stem()), file));
//         }
//         try {
//             file->linkNames.insert(std::pair<std::string, Entry*>(to_lower(file->tags.at("title").at(0)), file));
//         } catch (const std::out_of_range& oor) { /* ignored */}
//     }
    logger < "done with meta-data from this file\n";
    return;
}


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
                    if (Args::get.flag(Flags::OVERRIDE)) {
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

void command_link (std::stringstream& out_stream, const Command& command, const Entry* active_entry) {
    
}

void command_time (std::stringstream& out_stream, const Command& command, const Entry* active_entry) {
    
    std::vector<std::string> time;
    if (command.curly_para.length == 0) {
        // get time from file entry
        try {
            time = active_entry->tags.at("time");
        } catch (const std::out_of_range& oor) {
            throw std::runtime_error("Error: '$time' command got not passed in the time in a file without a 'time' tag");
        }
    } else {
        // get time from parameter
        time = split(command.curly_para.get(), ',');
    }
    
    if (time.size() == 0) {
        throw std::runtime_error("Error: didnt catch the time in the '$time' command properly");
    }
    
    /* Style of class="timestamp"
     * mark { 
     *   background-color: #DAAA75;
     *   color: black;
     *   font-weight: bold;
     * }
     */

    
    out_stream << "<mark class=\"timestamp\">[";
    
    if (time.size() >= 3 && time[2].length() > 0) {
        out_stream << time[2] << ".";
    }
    
    if (time.size() >= 2 && time[1].length() > 0) {
        if (time[1][0] == '\\') {
            out_stream << time[1].substr(1) << ".";
        } else if (time[1][0] == 'm') {
            size_t num_pos = 1;
            size_t num = std::stoul(time[1], &num_pos);
            
            if (num_pos <= 1) {
                throw std::runtime_error("Error: '$time' command cant complete the requested month-translation for '" + time[1] + "'");
            }
            
            if (ConfigConstants::months.size() <= num) {
                throw std::runtime_error("Error: '$time' command cant complete the requested month-translation for '" + time[1] + "'\n\tmonth number out of range");
            }
            
            out_stream << " " << ConfigConstants::months[num] << " ";
        } else {
            out_stream << time[1] << ".";
        }
    }
    
    if (time.size() >= 1 && time[0].length() > 0) {
        // if last character is a digit, concat the default calendar defined in 'constants.hpp'
        if (time[0][time[0].length() - 1] > '0' && time[0][time[0].length() - 1] < '9') {
            out_stream << time[0] << ConfigConstants::calendar;
            
        // if last character is a '_' omit the calendar (and also the '_')
        } else if (time[0][time[0].length() - 1] == '_') {
            time[0].pop_back();
            out_stream << time[0];
        } else {
            out_stream << time[0];
        }
    }
    
    out_stream << "]</mark>";
}


void process_command () {
    for (std::pair<std::string, Entry*> it : entries) {
        // if not wikiph: skip
        if (!it.second->phrased) {
            continue;
        }
        
        Logger::out < "start phrasing " < it.second->destination.string() < "\n";
        
        StrRange work_string(it.second->content[0]);
        
        Logger::out << "1st StrRange constructed\n";
        
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
        
        Logger::out << "1st start command suspected at pos " + std::to_string(pos) + "\n";
        
        Command command(work_string, pos);
        
        if (command.type != Command::CommandType::Start) {
            //! TODO: extent the message with file information
            throw std::runtime_error("Error: expected Start command (nr. "
                                    + std::to_string(static_cast<int>(Command::CommandType::Start))
                                    + "), but got nr. "
                                    + std::to_string(static_cast<int>(command.type))
                                    + "\n");
        }
        
        Logger::out << "1st start command found\n";
        
        
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
                        logger << "Warning: dubious escape character '\\' at end of file (" + it.second->source.string() + ") gets ignored\n";
                        break;
                    }
                    
                    content_builder[static_cast<size_t>(active_block)] << work_string[1];
                    work_string.consume_to(2);
                    
                } else if (work_string[0] == '$') {
                    
                    
                    try {
                        command = Command(work_string, 0);
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Fatal Error in file " + it.second->source.string() + ":\n" + e.what());
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
