
#include "Buffer.h"
#include "string.h"

Buffer::Buffer() {
    currentArrayElement = array;
    bytesFilled = 0;
    nextRecord = array;
}

Buffer::Buffer(Buffer& buffer) {
    bytesFilled = buffer.bytesFilled;
    std::memcpy(array, buffer.array, bytesFilled);
    currentArrayElement = array + bytesFilled;
    nextRecord = array + (buffer.nextRecord - buffer.array);
    int w = (int)(buffer.nextRecord - buffer.array);
    int x = w;
}

int Buffer::appendData(char *arr, int bytes) {
    if (this->bytesFilled + bytes > Capacity) {
        return 0;
    }
    std::memcpy(this->currentArrayElement, arr, bytes);
    this->bytesFilled += bytes;
    this->currentArrayElement = array + this->bytesFilled;
    return bytes;
}

void Buffer::clear() {
    this->bytesFilled = 0;
    this->currentArrayElement = array;
    this->nextRecord = array;
}

int Buffer::getBytesFilled() {
    return this->bytesFilled;
}

int Buffer::writeToFile(const char* name, int bytesToWrite) {
    if (bytesToWrite > bytesFilled) {
        return 0;
    }
    FILE* file = fopen(name, "a");
    if (file == nullptr) {
        return -1;
    }

    fwrite(array, sizeof(char), bytesToWrite, file);
    fclose(file);

    return bytesToWrite;
}

void Buffer::writeAllToFile(const char* name) {
    writeToFile(name, this->bytesFilled);
}

void Buffer::writeAndClear(const char* name) {
    writeAllToFile(name);
    clear();
}

int Buffer::readFromFile(const char* name, int fileOffset) {
    FILE* file = fopen(name, "r");
    if (file == nullptr) {
        return -1;
    }

    clear();

    fseek(file, -fileOffset, SEEK_END);
    long int bytesToRead = ftell(file);
    fseek(file, fileOffset, SEEK_SET);
    bytesToRead = std::min((int)bytesToRead, (int) Capacity);

    if (bytesToRead >= Capacity) {
        fread(array, sizeof(char), Capacity, file);
    } else if (bytesToRead > 0) {
        fread(array, sizeof(char), bytesToRead, file);
    }
    bytesFilled = (int) bytesToRead;
    nextRecord = array;

    fclose(file);
    return bytesToRead;
}

char *Buffer::getNextRecord() {
    if (nextRecord == array + bytesFilled)
        return nullptr; // lub wczytaj dalej plik
    nextRecord = nextRecord + 8;
    return nextRecord - 8;
}