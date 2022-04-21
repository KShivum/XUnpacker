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
    char emptySpace[32];
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
        //fs::create_directories(dirNameString);
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

class FileInfo
{
public:
    uint32_t dirIndex;
    uint8_t fileNameSize;
    string fileName;
    uint32_t dataOffset;
    uint32_t dataSize;
};

void Pack(string directory)
{
    map<int, string> directories;
    if (directory.at(directory.size() - 1) == '/')
    {
        directory.replace(directory.size() - 1, 1, "\0");
    }
    directories.insert(pair<int, string>(0, directory));
    vector<FileInfo> files;
    int dirIndex = 1;
    int fileSize = 0;
    cout << "------------------Scanning Files and Directories------------------" << endl;
    for (auto &p : std::filesystem::recursive_directory_iterator(directory))
    {

        if (p.is_directory())
        {
            /*
            cout << "Found Directory: " << p.path().string() << endl;
            directories.insert(pair<int, string>(dirIndex, p.path().string()));
            dirIndex++;
            */
        }
        else
        {

            FileInfo file;
            cout << "Found File: " << p.path().string() << endl;
            file.fileName = p.path().filename().string();
            if (strcasecmp(file.fileName.c_str(), "base.x") == 0)
            {
                cout << "Error: base.x is a reserved file name" << endl;
                continue;
            }
            else if (strcasecmp(file.fileName.c_str(), "Unpacker") == 0 || strcasecmp(file.fileName.c_str(), "Unpacker.exe") == 0)
            {
                cout << "Error: Unpacker is a reserved file name" << endl;
                continue;
            }
            string currentDirectory;
            currentDirectory = p.path().string();
            currentDirectory = currentDirectory.substr(0, currentDirectory.find_last_of("/"));
            bool foundDir = false;
            for (auto &i : directories)
            {
                string second(i.second);
                if (currentDirectory.compare(second) == 0)
                {
                    file.dirIndex = i.first;
                    foundDir = true;
                }
            }
            if (!foundDir)
            {
                directories.insert(pair<int, string>(dirIndex, currentDirectory));
                file.dirIndex = dirIndex;
                dirIndex++;
            }
            file.fileNameSize = file.fileName.size();
            file.fileName = file.fileName.erase(file.fileNameSize);
            files.push_back(file);
            fileSize += file.fileNameSize + 1 + 4 + 4 + 4;
        }
    }
    vector<char> fullFileData;
    int currentOffset = 24 + 32;
    cout << "------------------Reading File Data------------------" << endl;
    // Read files and store data into vector
    for (int i = 0; i < files.size(); i++)
    {
        cout << "Reading File: " << files[i].fileName << endl;
        ifstream file;
        string fileDirectory = directories.at(files[i].dirIndex);
        file.open(fileDirectory + "/" + files[i].fileName, ios::binary | ios::in);
        // Stores the file data in the vector
        file.seekg(0, ios::end);
        files[i].dataSize = file.tellg();
        file.seekg(0, ios::beg);
        files[i].dataOffset = currentOffset;
        char *fileData = (char *)malloc(files[i].dataSize);
        file.read(fileData, files[i].dataSize);
        fullFileData.insert(fullFileData.end(), fileData, fileData + files[i].dataSize);
        currentOffset += files[i].dataSize;
        file.close();
    }

    // Create the header
    uint32_t packId = (*((uint32_t *)"NORK"));
    uint32_t verId = 0x001;
    uint32_t dirCount = directories.size() - 1;
    uint32_t dirOffset = fullFileData.size() + 24 + 32 + fileSize;
    uint32_t fileCount = files.size();
    uint32_t fileOffset = fullFileData.size() + 24 + 32;
    char emptySpace[32];
    memset(emptySpace, 0, 32);
    ofstream outFile;
    outFile.open(directory + "/" + "base.x", ios::binary | ios::out);
    outFile.write((char *)&packId, sizeof(uint32_t));
    outFile.write((char *)&verId, sizeof(uint32_t));
    outFile.write((char *)&dirCount, sizeof(uint32_t));
    outFile.write((char *)&dirOffset, sizeof(uint32_t));
    outFile.write((char *)&fileCount, sizeof(uint32_t));
    outFile.write((char *)&fileOffset, sizeof(uint32_t));
    outFile.write(emptySpace, 32);
    cout << "------------------Writing File Data------------------" << endl;
    for (int i = 0; i < fullFileData.size(); i++)
    {
        outFile.write((char *)&fullFileData[i], sizeof(char));
    }
    cout << "------------------Writing File Info------------------" << endl;
    for (int i = 0; i < files.size(); i++)
    {
        outFile.write((char *)&files[i].dirIndex, sizeof(uint32_t));
        outFile.write((char *)&files[i].fileNameSize, sizeof(uint8_t));
        outFile.write(files[i].fileName.c_str(), files[i].fileNameSize);
        outFile.write((char *)&files[i].dataOffset, sizeof(uint32_t));
        outFile.write((char *)&files[i].dataSize, sizeof(uint32_t));
    }
    cout << "------------------Writing Directory Info------------------" << endl;
    for (int i = 1; i < directories.size(); i++)
    {
        size_t pos = directories.at(i).find(directory);
        if (pos != string::npos)
        {
            if (pos != 0)
            {
                continue;
            }
            directories.at(i).replace(0, directory.size(), "");
            if (directories.at(i).at(0) == '/')
            {
                directories.at(i).replace(0, 1, "");
            }
        }
    }
    for (int i = 1; i < directories.size(); i++)
    {
        uint8_t dirSize = directories.at(i).size();
        outFile.write((char *)&dirSize, sizeof(uint8_t));
        char *dirName = (char *)malloc(dirSize);
        strcpy(dirName, directories.at(i).c_str());
        outFile.write(dirName, dirSize);
    }
    outFile.close();
}

int main(int argc, char **argv)
{
    if (argc != 3 || strcmp(argv[1], "--help") == 0)
    {
        Help();
        return 0;
    }

    if (strcmp(argv[1], "-u") != 0 && strcmp(argv[1], "-p") != 0)
    {
        cout << "Invalid functional argument" << endl;
        Help();
        return 0;
    }

    if (strcmp(argv[1], "-u") == 0)
    {
        string directory(argv[2]);
        UnPack(directory);
    }
    else
    {
        string directory(argv[2]);
        Pack(directory);
    }

    return 0;
}