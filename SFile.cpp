
#include "SFile.h"

SFile::SFile() {}

SFile::SFile(const char *name) {
    fileName = new char [strlen(name)];
    strcpy(fileName, name);

    buffer = new Buffer();
    bufferOffset = fileOffset = 0;
}

SFile::SFile(SFile &file) {
    fileName = new char[strlen(file.fileName)];
    strcpy(fileName, file.fileName);

    buffer = new Buffer(*file.buffer);
    bufferOffset = file.bufferOffset;
    fileOffset = file.fileOffset;
}

void SFile::cleanUpFile() {
    deleteFile();
    createFile();
}

void SFile::deleteFile() {
    remove(fileName);
}

void SFile::createFile() {
    FILE * file = fopen(fileName, "r");
    fclose(file);
}

char *SFile::getNextRecord() {
    char* next = buffer->getNextRecord();
    if (next == nullptr) {
        int read = buffer->readFromFile(fileName, fileOffset);
        if (read == 0) {
            return nullptr;
        }
        else if (read > 0) {
            if (buffer->getBytesFilled() >= 0)
                fileOffset += read;
            fileReads++;
            bufferOffset = 0;
            next = buffer->getNextRecord();
        }
    }
    bufferOffset += 8;
    return next;
}

long SFile::getDiskOperations() {
    return fileWrites + fileReads;
}

void SFile::clearBuffer() {
    buffer->clear();
    bufferOffset = 0;
    fileOffset = 0;
}

void SFile::writeToFile(char *data, int bytes) {
    int bytesAdded = buffer->appendData(data, bytes);
    if (bytesAdded == 0) {
        buffer->writeAndClear(fileName);
        fileWrites++;
        bytesAdded = buffer->appendData(data, bytes);
        fileOffset += Buffer::Capacity;
    }
    bufferOffset += bytesAdded;
}

void SFile::writeRemaining() {
    buffer->writeAndClear(fileName);
    fileWrites++;
}

int SFile::getFileSize() {
    FILE* file = fopen(fileName, "r");
    if (file == nullptr) {
        return -1;
    }
    fseek(file, 0, SEEK_END);
    return (int) ftell(file);
}

SFile::~SFile() {
    delete buffer;
    delete[] fileName;
}