

void phraser_main () {
    std::cout << "parent from " << (fs::current_path() /= "..") << " is " << (fs::current_path() /= "..").parent_path() << '\n';
    for (const fs::directory_entry& dir_entry : std::filesystem::recursive_directory_iterator(fs::current_path() /= "testbox")) {
        std::cout << "root: " << dir_entry.path().root_path() << std::endl;
        std::cout << "The parent path of " << dir_entry.path()
                  << " is " << dir_entry.path().parent_path() << '\n';
    }
}
