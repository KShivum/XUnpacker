#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <map>
using namespace std;
// namespace fs = std::filesystem;
namespace fs = std::filesystem;
int main()
{
    uint32_t packId, verId, dirCount, dirOffset, fileCount, fileOffset;
    uint8_t emptySpace[32];
    char test[1000];
    ifstream baseFile;
    baseFile.open("base.x", ios::binary | ios::in);

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
        directoryMap.insert(pair<int, string>(currentDirectories+1, dirNameString));
        currentDirectories++;
    }

    cout << endl << endl << "------------------Creating Files------------------" << endl;

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

    
    return 0;
}