#include <iostream>
#include <stdio.h>
#include <fstream>
#include <filesystem>
#include <string.h>
#include <vector>
#include <map>
using namespace std;
// namespace fs = std::filesystem;
namespace fs = std::filesystem;

void Help()
{
    cout << "This program unpacks and packs ARTS's .x file" << endl
         << endl;
    cout << "Usage:" << endl;
    cout << "\tUnpacker[.exe] {-u, -p} {directory}" << endl;
    cout << "\t-u for unpacking and the directory to the base.x file" << endl;
    cout << "\t-p for packing and the directory to the directory to be packed" << endl;
}

void UnPack(string directory)
{
    uint32_t packId, verId, dirCount, dirOffset, fileCount, fileOffset;
    uint8_t emptySpace[32];
    char test[1000];
    ifstream baseFile;
    baseFile.open(directory, ios::binary | ios::in);

    baseFile.read((char *)&packId, sizeof(uint32_t));
    baseFile.read((char *)&verId, sizeof(uint32_t));
    baseFile.read((char *)&dirCount, sizeof(uint32_t));
    baseFile.read((char *)&dirOffset, sizeof(uint32_t));
    baseFile.read((char *)&fileCount, sizeof(uint32_t));
    baseFile.read((char *)&fileOffset, sizeof(uint32_t));
    baseFile.read((char *)emptySpace, 32);

    // Create directories

    int currentDirectories = 0;
    int directoryOffset = 0;
    map<int, string> directoryMap;
    string currentDirectory;
    currentDirectory = fs::current_path().string();
    directoryMap.insert(pair<int, string>(0, currentDirectory));
    while (currentDirectories < dirCount)
    {
        baseFile.seekg(dirOffset + directoryOffset);
        uint8_t dirNameLength;
        baseFile.read((char *)&dirNameLength, sizeof(uint8_t));
        char dirName[dirNameLength + 1];
        baseFile.read((char *)dirName, dirNameLength);
        dirName[dirNameLength] = '\0';
        string dirNameString(dirName);
        while (dirNameString.find("\\") != string::npos)
        {
            dirNameString.replace(dirNameString.find("\\"), 1, "/");
        }
        cout << "Creating Directory: " << dirNameString << endl;
        fs::create_directories(dirNameString);
        directoryOffset += dirNameLength + 1;
        directoryMap.insert(pair<int, string>(currentDirectories + 1, dirNameString));
        currentDirectories++;
    }

    cout << endl
         << endl
         << "------------------Creating Files------------------" << endl;

    // Create files
    int currentFiles = 0;
    int fileCurrentOffset = 0;
    while (currentFiles < fileCount)
    {
        baseFile.seekg(fileOffset + fileCurrentOffset);
        uint32_t dirIndex, dataOffset, dataSize;
        uint8_t fileSize;

        baseFile.read((char *)&dirIndex, sizeof(uint32_t));
        baseFile.read((char *)&fileSize, sizeof(uint8_t));
        char *fileName = (char *)malloc(fileSize + 1);
        baseFile.read(fileName, fileSize);
        baseFile.read((char *)&dataOffset, sizeof(uint32_t));
        baseFile.read((char *)&dataSize, sizeof(uint32_t));

        baseFile.seekg(dataOffset);
        char *fileData = (char *)malloc(dataSize);
        baseFile.read(fileData, dataSize);

        fileName[fileSize] = '\0';
        string fileNameString(fileName);

        string dirName = directoryMap.at(dirIndex);

        ofstream file;
        file.open(dirName + "/" + fileNameString, ios::binary | ios::out);
        cout << "Creating File: " << dirName << "/" << fileNameString << endl;
        file.write(fileData, dataSize);
        file.close();
        currentFiles++;
        fileCurrentOffset += fileSize + 4 + 1 + 4 + 4;
    }
}

void Pack(string directory)
{
    map<int, string> directories;
    directories.insert(pair<int, string>(0, directory))
    vector<string> files;
    for (auto &p : std::filesystem::recursive_directory_iterator(directory))
    {
        if (p.is_directory())
        {

        }
        else
        {
            files.push_back(p.path().filename())
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
    {
        Help();
    }

    if (strcmp(argv[1], "-u") != 0 && strcmp(argv[1], "-p") != 0)
    {
        cout << "Invalid functional argument" << endl;
        Help();
    }

    if (strcmp(argv[1], "-u") == 0)
    {
        string directory(argv[2]);
        UnPack(directory);
    }

    return 0;
}