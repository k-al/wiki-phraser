

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

    static std::unordered_map<std::string, Entry*> linkNames;


    bool phrased = false;
    bool directory = false;
    fs::path source;
    fs::path destination;
    std::unordered_set<std::string> links; // set of paths that Entry links to
    std::unordered_map<std::string, std::vector<std::string>> tags;
    std::array<std::string, static_cast<size_t>(Block::number)> content;

    void set_content (std::string content, Block block) {
        if (block != Block::number) {
            this->content[static_cast<size_t>(block)].swap(content);
        }
    }

    //void write_content (std::string content, Block to) {
    //    if (to == Block::number) {
    //        // logger << "WARNING: writing to unspecified block in file " << source << "\n"; // kein zugriff auf logger
    //        return;
    //    }
    //    std::swap(this->content[static_cast<size_t>(to)], content);
    //}
};

std::unordered_map<std::string, Entry*> Entry::linkNames;

namespace Phraser {

    std::string image (fs::path path) {
        return "<div class=\"spacer\"><img class=\"characterPic\" src=\"" + path.string() + "\"></div>";
    }

    std::string dropdownTitle (fs::path path, std::string name) {
        return "<div><a class=\"navtitle\" href=\"" + path.string() + "\" target=\"content\"><div>" + name + "</div></a></div>";
    }

    std::string dropdownLink (fs::path path, std::string name) {
        return "<a href=\"" + path.string() + "\" target=\"content\">\n"
               "     <div class=\"linkitem\">" + name + "</div>\n"
               "</a>";
    }



    static std::unordered_map<std::string, Entry*> entries;
    static Args arguments;
    static Logger logger;

    /**
     * Computes the tags of the given wikiph file and places them in the Entry\n
     * @param file the Entry to store the data in
     * @param buff the input file string to get the tags from
     */
    void computeMetadata(Entry* file, const std::string& buff) {
        size_t position = 0;

        size_t first_command = buff.find_first_of("$", position);
        size_t first_tag = buff.find_first_of(":", position);

        while (true) {
            first_command = buff.find_first_of("$", position);
            first_tag = buff.find_first_of(":", position);

            if (first_command == std::string::npos && first_tag == std::string::npos) {
                logger << "ERROR: no content section\n";
                break;
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
                        if (file->linkNames.find(linkn) != file->linkNames.end()) {
                            logger << "WARNING: multiple uses of linkname '" << linkn << "'\n";
                        }

                        file->linkNames[linkn] = file;
                    }
                }

                position = first_tag + 1;
            } else {
                if (buff.substr(first_command + 1, first_command + 6 - first_command) == "start{") {
                    file->content[0] = buff.substr(first_command);
                    return;
                } else {
                    logger << "ERROR: incorrect command in meta-section\n";
                    break;
                }
            }
        }
        exit(1);
    }

    /**
     * Reads the input Files and places the tags in the given Entry\n
     * also places the command section in content at pos 0
     * @param file the file Entry to phrase
     */
    void phrase_tags (Entry* file) {
        // read baseHtmlFiles
        // std::ifstream baseLayout;
        // baseLayout.open(fs::path("baseHtmlFiles/baseLayout.html"));
        logger < "\n\nPhrasing files\n";

        std::ifstream inFile;
        inFile.open(file->source);

        // needed at writing at destination:
        //
        // static std::ifstream informationLayout;
        // informationLayout.open(fs::path("baseHtmlFiles/informationLayout.html"));
        // std::ofstream informationFile;
        // informationFile.open(file->destination);

        const std::string buff = read_file(inFile);

        computeMetadata(file, buff);
        
        logger << "\nWARNING: not implemented yet\n";
    }

    // TODO: write content in einem stück
    std::string checkCommands(Entry* file, Entry::Block start, std::string buff, size_t open, size_t close, bool list) {
        size_t pos = open;
        std::string content = "";

        if(list) {
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
                        if(file->linkNames.count(links) == 1) {
                            content += "<a href=\"" + file->linkNames.at(links)->destination.string() + "\">" + links + "</a>";
                        } else {
                            // TODO: Error message
                        }
                    } else {
                        for (std::string link : split(links, ',')) {
                            if(file->linkNames.count(link) == 1) {
                                content += "<a href=\"" + file->linkNames.at(link)->destination.string() + "\">" + link + "</a>";
                            } else {
                                // TODO: Error message
                            }
                        }
                    }
                } else if (links.find("$tags") != std::string::npos) {
                    for (std::string link: file->tags.at(links.substr(links.find_first_of('{') + 1,
                                                                      links.find_last_of('}') -
                                                                      links.find_first_of('{') - 1))) {
                        if (file->linkNames.count(link) == 1) {
                            content += "<a href=\"" + file->linkNames.at(link)->destination.string() + "\">" + link + "</a>";
                        } else {
                            logger << YELLOW << "WARNING: Link to " << link << " not found" << RESET;
                            content += link;
                        }
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

    /**
     *
     */
    void phrase_commands() {

        // second iteration over entries for the commands
        for (auto it : entries) {
            // if not wikiph: skip
            if (!it.second->phrased) {
                continue;
            }
            const std::string buff = it.second->content[0];

            checkCommands(it.second, Entry::Block::number, buff, 0, buff.size() - 1, false);

            logger << it.first;
        }
    }

    /**
     * the main of the phraser\n
     * scans dirs for files to phrase images to copy...\n
     * executes phrase_file to phrase the wikiph files
     * @param args
     * @param log
     */
    void phraser_main (Args args, Logger log) {
        
        arguments = args;
        logger = log;
        
        //! this could be a problem:
        std::cout << "Problem: parent from " << (fs::current_path() / "..") << " is " << (fs::current_path() / "..").parent_path() << '\n';

        
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
                    
                    phrase_tags(new_file);
                    
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
                logger << "WARNING: Can't handle " << dir_entry.path() << "\n \t Softlinks are not supported.\n";
            }
        }
        phrase_commands();
    }
}
