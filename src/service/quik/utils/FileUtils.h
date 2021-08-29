//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_FILEUTILS_H
#define QUIK_CONNECTOR_FILEUTILS_H

#include <string>
#include <set>
#include <fstream>

using namespace std;

class FileUtils {
public:
    static void createdDirs(const string& path);

    static set<string> getFiles(string& directory);

    static string readFile(string& filePath);

    static uint64_t getFileSize(string& filePath);

private:
    static void createdDir(const string& path);
};

#endif //QUIK_CONNECTOR_FILEUTILS_H
