#include<iostream>
#include<fstream>
#include <filesystem>
#include <string>
using namespace std;
//namespace fs = std::filesystem;
namespace fs = std::filesystem;
int main()
{
    uint32_t packId, verId, dirCount, dirOffset, fileCount, fileOffset;
    uint8_t emptySpace[32];
    char test[1000];
    ifstream baseFile;
    baseFile.open("base.x", ios::binary | ios::in);

    baseFile.read((char*)&packId, sizeof(uint32_t));
    baseFile.read((char*)&verId, sizeof(uint32_t));
    baseFile.read((char*)&dirCount, sizeof(uint32_t));
    baseFile.read((char*)&dirOffset, sizeof(uint32_t));
    baseFile.read((char*)&fileCount, sizeof(uint32_t));
    baseFile.read((char*)&fileOffset, sizeof(uint32_t));
    baseFile.read((char*)emptySpace, 32);

    //Create directories

    int currentDirectories = 0;
    int directoryOffset = 0;
    while(currentDirectories < dirCount)
    {
        baseFile.seekg(dirOffset + directoryOffset);
        uint8_t dirNameLength;
        baseFile.read((char*)&dirNameLength, sizeof(uint8_t));
        char dirName[dirNameLength + 1];
        baseFile.read((char*)dirName, dirNameLength);
        dirName[dirNameLength] = '\0';
        string dirNameString(dirName);
        while(dirNameString.find("\\") != string::npos)
        {
            dirNameString.replace(dirNameString.find("\\"), 1, "/");
        }
        fs::create_directories(dirNameString);
        directoryOffset += dirNameLength + 1;
        currentDirectories++;
    }






    /*
    int firstFile = fileOffset;
    //Read file data
    baseFile.seekg(firstFile);
    uint32_t dirIndex, dataOffset, dataSize;
    uint8_t fileSize;

    baseFile.read((char*)&dirIndex, sizeof(uint32_t));
    baseFile.read((char*)&fileSize, sizeof(uint8_t));
    char *fileName = (char*)malloc(fileSize);
    baseFile.read(fileName, fileSize);
    baseFile.read((char*)&dataOffset, sizeof(uint32_t));
    baseFile.read((char*)&dataSize, sizeof(uint32_t));

    baseFile.seekg(dataOffset);
    char *fileData = (char*)malloc(dataSize);
    baseFile.read(fileData, dataSize);
    cout << fileName << endl;
    cout << fileData << endl;
    */
    return 0;

}