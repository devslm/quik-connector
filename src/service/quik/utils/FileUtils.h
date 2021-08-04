//
// Created by Sergey on 23.07.2021.
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

private:
    static void createdDir(const string& path);
};

#endif //QUIK_CONNECTOR_FILEUTILS_H
