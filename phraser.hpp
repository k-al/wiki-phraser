

struct Entry {
    enum class Block {
        Main = 0,
        Side,
        number
    };
    
    Entry () {};
    
    Entry (Entry&& rhs) {
        this->phrased = rhs.phrased;
        this->source = std::move(rhs.source);
        this->destination = std::move(rhs.destination);
        this->links = std::move(rhs.links);
        this->tags = std::move(rhs.tags);
        this->content = std::move(rhs.content);
    }
    
    bool phrased = false;
//     bool directory = false;
    fs::path source;
    fs::path destination;
//     std::unordered_set<fs::path> links;
//     std::unordered_map<std::string, std::vector<std::string>> tags;
//     std::array<std::string, static_cast<size_t>(Block::number)> content;
};

namespace Phraser {
    static std::unordered_map<fs::path, Entry> entries(11);
    static Args arguments;
    static Logger logger;
    
    
    void phrase_file (Entry& file) {
        logger << "WARNING: not implemented yet\n";
    }

    void phraser_main (Args args, Logger log) {
        
        arguments = args;
        logger = log;
        
        //! this could be a problem:
        std::cout << "Problem: parent from " << (fs::current_path() /= "..") << " is " << (fs::current_path() /= "..").parent_path() << '\n';
        
        
        for (const fs::directory_entry& dir_entry : std::filesystem::recursive_directory_iterator(arguments.source)) {
            logger < "\n\nFile: " < dir_entry.path() < "\n";
            
            fs::path rel_path = dir_entry.path().lexically_relative(arguments.source);
            fs::path dest_path = (arguments.dest /= rel_path).lexically_normal();
            
            
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
                if (rel_path.extension() == "wikiph") {
                    dest_path.replace_extension("html");
                    logger < "Is a phrasable file and will be phrased to " < dest_path < "\n";
                    
                    Entry new_file;
                    new_file.source = dir_entry.path();
                    new_file.destination = dest_path;
                    
                    phrase_file(new_file);
                    
                    entries.insert(std::make_pair<fs::path, Entry>(std::move(rel_path), std::move(new_file)));
                } else if (rel_path.extension() == "txt") {
                    logger < "Is a .txt file and will be copied to " < dest_path < "\nWARNING: Do you really want a .txt on your website?\n";
                    fs::directory_entry dest_entry(dest_path);
                    if (dest_entry.exists()) {
                        logger < "Destination file allready exists";
                        if (arguments.get(Flags::OVERRIDE)) {
                            logger < ", but overwrite it because of the '-o' Flag\n";
                            fs::copy_file(dir_entry.path(), dest_path, fs::copy_options::overwrite_existing);
                        }
                    }
                }
            }
        }
    }
}
