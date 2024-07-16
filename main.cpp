#include <iostream>
#include <stdio.h>
#include "SFile.h"


int compareBoard(char* arr1, char* arr2) {
    for (int i=0; i<7; i++) {
        if (arr1[i] > arr2[i]) {
            return 1;
        }
        if (arr1[i] < arr2[i]) {
            return -1;
        }
    }
    return 0;
}

void recordCopy(char* dest, char* src) {
    for (int i=0; i<8; i++) {
        dest[i] = src[i];
    }
}

void printFile(int phase) {
    FILE* file = fopen("tape3.txt", "r");
    char arr[10];
    printf("(-) Rekordy po %i fazie sortowania (-)\n", phase);
    while (fgets(arr, 10, file)) {
        printf("%s", arr);
    }
    printf("(-) ------------------ (-)\n");
    fclose(file);
}

void clearTapes(SFile* tape1, SFile* tape2, SFile* tape3) {
    if (tape1 != nullptr)
        tape1->cleanUpFile();
    if (tape2 != nullptr)
        tape2->cleanUpFile();
    if (tape3 != nullptr)
        tape3->cleanUpFile();
}

void clearBuffers(SFile* tape1, SFile* tape2, SFile* tape3) {
    if (tape1 != nullptr)
        tape1->clearBuffer();
    if (tape2 != nullptr)
        tape2->clearBuffer();
    if (tape3 != nullptr)
        tape3->clearBuffer();
}

int FindSeriesInFile(SFile file) {
    file.clearBuffer();
    int len = 0;
    char last[9] = "";
    while (true) {
        char* data = file.getNextRecord();
        if (data == nullptr) {
            break;
        }
        int cmp = compareBoard(last, data);
        recordCopy(last, data);

        if (cmp <= 0)
            len++;
        else
            break;
    }
    return len;
}

void Merge(SFile* tape1, SFile* tape2, SFile* tape3) {
    clearBuffers(tape1, tape2, tape3);
    clearTapes(nullptr, nullptr, tape3);
    char *name1, *name2;

    name1 = tape1->getNextRecord();
    name2 = tape2->getNextRecord();

    char lastName1[8] = "",
         lastName2[8] = "";

    int cmp1, cmp2;
    while (name1 != nullptr || name2 != nullptr) {
        if (name1 != nullptr)
            cmp1 = compareBoard(lastName1, name1);
        else
            cmp1 = 1;
        if (name2 != nullptr)
            cmp2 = compareBoard(lastName2, name2);
        else
            cmp2 = 1;

        if (cmp1 < 0) {
            recordCopy(lastName1, name1);
        }
        if (cmp2 < 0) {
            recordCopy(lastName2, name2);
        }
        if (cmp1 <= 0 && cmp2 <= 0) { // name1 i name2 porownanie
            int cmp = compareBoard(name1, name2);
            if (cmp <= 0) {
                tape3->writeToFile(name1, 8);
                name1 = tape1->getNextRecord();
            } else {
                tape3->writeToFile(name2, 8);
                name2 = tape2->getNextRecord();
            }
        } else if (cmp1 <= 0 && cmp2 > 0) { // dalej porownoje name1
            tape3->writeToFile(name1, 8);
            name1 = tape1->getNextRecord();
        } else if (cmp1 > 0 && cmp2 <= 0) { // dalej porownoje name1
            tape3->writeToFile(name2, 8);
            name2 = tape2->getNextRecord();
        }

        if (cmp1 > 0 && cmp2 > 0) { // koniec serji
            char temp[8] = "";
            recordCopy(lastName1, temp);
            recordCopy(lastName2, temp);
        }
    }
    tape3->writeRemaining();
}

void DistrubuteTape(SFile* source, SFile* tape1, SFile* tape2) {
    char last_data[8];
    int tape = 0;
    int bytesToRead = source->getFileSize();
    clearTapes(tape1, tape2, nullptr);
    clearBuffers(nullptr, nullptr, source);

    while (bytesToRead >= 0) {
        char* data = source->getNextRecord();
        if (data == nullptr) {
            break;
        }
        int cmp = compareBoard(last_data, data);
        recordCopy(last_data, data);

        if (cmp == 1) {
            if (tape == 0) {
                tape = 1;
            } else {
                tape = 0;
            }
        }

        if (tape == 0) {
            tape1->writeToFile(data, 8);
        } else {
            tape2->writeToFile(data, 8);
        }

        bytesToRead -= 8;
    }

    tape1->writeRemaining();
    tape2->writeRemaining();
}

void generateSingleRecord(char* arr) {
    int letter;
    for (int i=0; i<7; i++) {
        if (i < 2) {
            letter = rand() % 26;
            arr[i] = char(letter + 'A');
        }
        else {
            letter = rand() % 10;
            arr[i] = char(letter + '0');
        }
    }
    arr[7] = '\n';
}

void generateFile(const char* name, int records) {
    char record[8];
    SFile newfile(name);
    for (int i=0; i<records; i++) {
        generateSingleRecord(record);
        newfile.writeToFile(record, 8);
    }
    newfile.writeRemaining();
}

int intToChar(char* arr, int val, char lastSymbol) {
    sprintf(arr, "%d", val);
    int len = strlen(arr);
    arr[len] = lastSymbol;
    return len+1;
}

struct sortInformation {
public:
    int phases;
    int operations;
};

sortInformation sort(char* fileName, int printRecords, int records) {
    SFile source(fileName);
    SFile tape1("tape1.txt");
    SFile tape2("tape2.txt");
    SFile tape3("tape3.txt");

    clearTapes(&tape1, &tape2, &tape3);

    int phase = 0;
    DistrubuteTape(&source, &tape1, &tape2);

    while (true) {
        Merge(&tape1, &tape2, &tape3);
        if (printRecords == 1)
            printFile(phase);
        phase++;
        if (FindSeriesInFile(tape3) == records)
            break;
        DistrubuteTape(&tape3, &tape1, &tape2);
    }

    sortInformation inf{};
    inf.operations = (int)(tape1.getDiskOperations() + tape2.getDiskOperations() + tape3.getDiskOperations());
    inf.phases = phase;
    return inf;
}

void generateTestData() {
    int N[] = {1000, 5000, 10000, 20000, 50000, 100000, 200000, 500000, 1000000};

    remove("test_result.txt");
    SFile result("test_result.txt");

    char buff[32];
    char name[] = "gen.txt";
    for (int i : N) {
        remove(name);
        generateFile(name, i);
        sortInformation inf = sort(name, 0, i);

        int len;
        len = intToChar(buff, i, ',');
        result.writeToFile(buff, len);
        len = intToChar(buff, inf.operations, ',');
        result.writeToFile(buff, len);
        len = intToChar(buff, inf.phases, '\n');
        result.writeToFile(buff, len);
    }

    result.writeRemaining();
}

int main() {

    srand(time(nullptr));
    setbuf(stdout, nullptr);

    char input[64];
    char fileName[64] = "gen.txt";
    int records;
    int printRecords = 0;

    printf("Czy chcesz wygenerowac plik, wpisac wlasne dane lub przeprowadzic test ? (gen/input/test)\n");
    scanf("%s", input);

    if (strcmp(input, "gen") == 0 || strcmp(input, "input") == 0) {
        if (strcmp(input, "gen") == 0) {
            printf("Podaj ilosc rekordow do wygenerowania\n");
            scanf("%s", input);
            records = atoi(input);

            remove(fileName);
            generateFile(fileName, records);
        }
        else {
            remove(fileName);
            FILE* file = fopen(fileName, "a");

            printf("Podaj kolejne rekordy do wpisania (format: [AA00000], \"q\" oznacza koniec wpisywania)\n");
            while (true) {
                scanf("%s", input);
                if (strcmp(input, "q") == 0)
                    break;
                else {
                    input[7] = '\n';
                    fwrite(input, sizeof(char), 8, file);
                }
            }

            fseek(file, 0, SEEK_END);
            records = (int) ftell(file)/8;
            fclose(file);
        }

        printf("Czy chcesz wyswietlac rekordy po kazdej fazie ? (t/n)\n");
        scanf("%s", input);
        if (strcmp(input, "t") == 0) {
            printRecords = 1;
        }

        sortInformation inf = sort(fileName, printRecords, records);

        printf("Disk operations: %i\n", inf.operations);
        printf("Phases: %i\n", inf.phases);

    }
    else if (strcmp(input, "test") == 0) {
        generateTestData();
    }

    return 0;
}
