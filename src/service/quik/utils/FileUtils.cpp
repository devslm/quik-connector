//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
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

set<string> FileUtils::getFiles(string& directory) {
    HANDLE dir;
    WIN32_FIND_DATA fileData;
    set<string> files;

    if ((dir = FindFirstFile((directory + "/*").c_str(), &fileData)) == INVALID_HANDLE_VALUE) {
        return files;
    }

    while (FindNextFile(dir, &fileData)) {
        if (strcmp(fileData.cFileName, ".") != 0 && strcmp(fileData.cFileName, "..") != 0) {
            files.insert(fileData.cFileName);
        }
    }
    return files;
}

string FileUtils::readFile(string& filePath) {
    string line;
    string fileData;

    ifstream file(filePath);

    if (file.is_open()) {
        while ( getline (file, line) ) {
            fileData += line + '\n';
        }
        file.close();
    }
    return fileData;
}
