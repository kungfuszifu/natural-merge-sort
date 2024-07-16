
#pragma once

#include <iostream>

class Buffer {
public:
    static const int Capacity = 4096;

private:
    char array[Capacity];
    char* currentArrayElement;
    int bytesFilled;
    char* nextRecord;

public:
    Buffer();

    Buffer(Buffer &buffer);

    int readFromFile(const char* name, int fileOffset);

    int writeToFile(const char* name, int bytesToWrite);
    void writeAllToFile(const char* name);
    void writeAndClear(const char* name);

    int appendData(char* arr, int bytes);

    char* getNextRecord();
    int getBytesFilled();

    void clear();

    ~Buffer() = default;
};

