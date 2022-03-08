

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

    /**
     *
     */
    void phrase_commands() {

        size_t pos = 0;
        Entry::Block write = Entry::Block::number;

        // second iteration over entries for the commands
        for (auto it : entries) {
            pos = 0;
            // if not wikiph: skip
            if (!it.second->phrased) {
                continue;
            }
            const std::string buff = it.second->content[0];
            while (true) {
                size_t first_command = buff.find_first_of('$', pos);

                if (first_command == std::string::npos) {
                    break;
                }
                // Build Html content
                std::string command = buff.substr(first_command + 1, buff.find_first_of('{', first_command) - first_command - 1);
                if(command == "start") {

                } else if (command == "time") {

                } else if (command == "subsection") {

                } else if (command == "list") {

                } else if (command == "tags") {

                } else if (command == "linknames") {

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

            }

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
                        logger < "WARNING: Do you really want a .txt on your website?\n";
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
