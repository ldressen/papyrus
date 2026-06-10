#include "FileBrowser.h"
#include "logger.h"

std::vector<std::string> FileBrowser::getCurrentPathDirFiles()
{
    std::vector<std::string> files;
    std::filesystem::path currentDir = std::filesystem::current_path();
    LOG_DEBUG() << "currDir: " << currentDir;
    
    for(auto const& dir_entry : std::filesystem::directory_iterator{currentDir}){
        files.push_back(dir_entry.path());
        LOG_DEBUG() << "file: " << dir_entry.path();
    }
    
    return files;
}
