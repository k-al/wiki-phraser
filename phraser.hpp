

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
    
    bool phrased = false;
    bool directory = false;
    fs::path source;
    fs::path destination;
    std::unordered_set<std::string> links; // set of paths that Entry links to
    std::unordered_map<std::string, std::vector<std::string>> tags;
    std::array<std::string, static_cast<size_t>(Block::number)> content;
};

namespace Phraser {
    static std::unordered_map<std::string, Entry*> entries;
    static Args arguments;
    static Logger logger;
    
    
    void phrase_file (Entry* file) {
        
        
        
        logger << "WARNING: not implemented yet\n";
    }

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
                
                //? maybe try it with constructor of std::pair
                entries.insert(std::make_pair<fs::path, Entry*>(rel_path.string(), &(*new_file))); // &* is to get a new rval(&&) from the pointer (no idea why copy is not workig)
                
                
                if (rel_path.extension() == ".wikiph") {
                    dest_path.replace_extension("html");
                    logger < "Is a phrasable file and will be phrased to " < dest_path < "\n";
                    
                    phrase_file(new_file);
                    
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
    }
}
