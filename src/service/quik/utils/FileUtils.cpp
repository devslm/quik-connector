//
// Created by Sergey on 23.07.2021.
//

#include "FileUtils.h"
#include <Windows.h>

void FileUtils::createdDir(const string& path) {
    int errorCode = CreateDirectory(path.c_str(), nullptr);

    if (ERROR_PATH_NOT_FOUND == errorCode) {
        throw runtime_error("Could not init logger because path: " + path + " not found!");
    }
}

void FileUtils::createdDirs(const string& path) {
    if (path.empty()) {
        return;
    }
    char * pathChars = new char [path.length()+1];

    std::strcpy(pathChars, path.c_str());

    string pathItems;

    for (int i = 0; i < path.length(); ++i) {
        if (pathChars[i] == '/') {
            createdDir(pathItems);
        }
        pathItems += pathChars[i];
    }
    createdDir(pathItems);
}
