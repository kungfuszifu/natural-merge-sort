
#pragma once

#include <iostream>
#include "Buffer.h"

class SFile {
private:
    Buffer* buffer;
    int bufferOffset;
    int fileOffset;
    char* fileName;

    int fileWrites = 0;
    int fileReads = 0;

    void createFile();
    void deleteFile();

public:
    SFile();
    SFile(const char* name);

    SFile(SFile &file);

    void writeToFile(char* data, int bytes);
    void writeRemaining();

    void clearBuffer();

    int getFileSize();

    long getDiskOperations();

    void cleanUpFile();
    //Buffer* readFromFile(int bytes);
    char* getNextRecord();

    ~SFile();
};

