#pragma once

#include <filesystem>
#include <iostream>
#include <vector>
class FileBrowser
{

public:
    FileBrowser() = default;
    std::vector<std::string> getCurrentPathDirFiles();
private:

    std::vector<std::string> currentPathDirFiles;

    std::filesystem::path mCurrentPath;
};