#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

struct stat s1, s2;
void mySeek(int fd, uint64_t offset);
void mySeek(int fd, uint64_t offset) {
    if(lseek(fd, offset, SEEK_SET) == -1) err(5, "cant seek");
}

void readWord(int fd1, int fd2, uint64_t pos, char* word);
void readWord(int fd1, int fd2, uint64_t pos, char* word) {
    uint32_t posOffset;
    mySeek(fd2, pos * sizeof(uint32_t));
    if(read(fd2, &posOffset, sizeof(posOffset)) != sizeof(posOffset)) err(7, "cant read offset");
    if(posOffset > s1.st_size) errx(8, "invalid offset");
    mySeek(fd1, posOffset);

    uint8_t byte;
    if(read(fd1, &byte, sizeof(byte)) != sizeof(byte)) err(9, "cant read");
    if(byte != 0x0) errx(9, "invalid format");
    uint8_t len = 0;
    int8_t bytesRead;

    while((bytesRead = read(fd1, &byte, sizeof(byte))) == sizeof(byte)) {
        if(len >= 64) errx(10, "to long word");
        word[len] = byte;
        if(byte == '\n') {
           word[len] = '\0';
           break;
        }
        len++;
    }
    if(bytesRead == 0 && word[len] != '\0') errx(11, "broken dic");
    if(bytesRead == -1) err(12, "cant read");
}

void writeDescription(int fd);
void writeDescription(int fd) {
    uint8_t byte;
    int8_t bytesRead;

    while((bytesRead = read(fd, &byte, sizeof(byte))) == sizeof(byte)) {
        if(byte == 0x0) break;

        if(write(1, &byte, sizeof(byte)) != sizeof(byte)) err(12, "cant write");
    }
    if(bytesRead == 0) errx(13, "Wrong gormat");
    if(bytesRead == -1) err(14, "cant read");
    byte = '\n';
    if(write(1, &byte, sizeof(byte)) != sizeof(byte)) err(13, "cant write");
}

void binarySearch(uint64_t wordsCount, char*searched, int fd1, int fd2);
void binarySearch(uint64_t wordsCount, char* searched, int fd1, int fd2) {
    int64_t start = 0, end = wordsCount - 1;
    int64_t mid;
    char word[64];
    int16_t cmp;

    while(start < end) {
        mid = (end - start) / 2 + start;

        readWord(fd1, fd2, mid, word);
        cmp = strcmp(searched, word);

        if(cmp < 0) {
            end = mid - 1;
        } else if(cmp > 0) {
            start = mid + 1;
        } else {
            writeDescription(fd1);
            return;
        }
    }

    char notFound[] = "word not found\n";
    if(write(1, notFound, 15) != 15) err(14, "cant write");

}

int main(int argc, char* argv[]) {

    if(argc != 4) errx(1, "3 params needed!");

    int fd1 = open(argv[2], O_RDONLY);
    if(fd1 == -1) err(2, "cant open");

    int fd2 = open(argv[3], O_RDONLY);
    if(fd2 == -1) err(2, "cant open");

    if(fstat(fd1, &s1) == -1) err(3, "cant stat");
    if(fstat(fd2, &s2) == -1) err(3, "cant stat");
    if(s2.st_size % sizeof(uint32_t) != 0) errx(4, "wrong index format");

    char* search = argv[1];
    uint64_t wordsCount = s2.st_size / sizeof(uint32_t);

    binarySearch(wordsCount, search, fd1, fd2);

    close(fd1);
    close(fd2);
    return 0;
}
