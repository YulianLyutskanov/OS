#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>

struct dataH {
    uint32_t magic, count;
};

struct compH {
    uint32_t magic1;
    uint16_t magic2;
    uint16_t reserved;
    uint64_t count;
};

struct compEl {
    uint16_t type;
    uint16_t reserved1, reserved2, reserved3;
    uint32_t offset1, offset2;
};

void mySeek(int fd, uint32_t offset);
void mySeek(int fd, uint32_t offset) {
    if(lseek(fd, offset, SEEK_SET) == -1) err(13, "cant seek");
}

uint64_t readFrom(int fd, uint32_t offset);
uint64_t readFrom(int fd, uint32_t offset) {
    mySeek(fd, offset);
    uint64_t res;
    if(read(fd, &res, sizeof(res)) != sizeof(res)) err(14, "cant read");
    return res;
}

bool shouldSwap(uint64_t first, uint64_t second, uint16_t type);
bool shouldSwap(uint64_t first, uint64_t second, uint16_t type) {
    if(type == 0) return first > second;
    if(type == 1) return first < second;

    return false;
}

void mySwap(int fd, uint32_t offset1, uint32_t offset2, uint64_t first, uint64_t second);
void mySwap(int fd, uint32_t offset1, uint32_t offset2, uint64_t first, uint64_t second) {
    mySeek(fd, offset1);
    if(write(fd, &second, sizeof(second)) != sizeof(second)) err(15, "cant write");
    mySeek(fd, offset2);
    if(write(fd, &first, sizeof(first)) != sizeof(first)) err(16, "cant write");
}

typedef struct dataH dataH_t;
typedef struct compH compH_t;
typedef struct compEl compEl_t;

int main(int argc, char* argv[]) {

    if(argc != 3) errx(1, "2 params needed");

    int fd1 = open(argv[1], O_RDWR);
    if(fd1 == -1) err(2, "Cant open");

    int fd2 = open(argv[2], O_RDONLY);
    if(fd2 == -1) err(2, "Cant open");

    struct stat s1, s2;
    if(fstat(fd1, &s1) == -1 || fstat(fd2, &s2) == -1) err(3, "cant stat");
    if(s1.st_size < (long int)sizeof(dataH_t)) errx(4, "file ivalid");
    if(s2.st_size < (long int)sizeof(compH_t)) errx(5, "file ivalid");

    dataH_t dataH;
    if(read(fd1, &dataH, sizeof(dataH_t)) != sizeof(dataH_t)) err(6, "cant read");
    if(dataH.magic != 0x21796F4A) errx(7, "file not in format");
    if(sizeof(dataH_t) + sizeof(uint64_t) * dataH.count != (long unsigned int)s1.st_size) errx(8, "invalid count");

    compH_t compH;
    if(read(fd2, &compH, sizeof(compH_t)) != sizeof(compH_t)) err(9, "cant read");
    if(compH.magic1 != 0xAFBC7A37 || compH.magic2 != 0x1C27)  errx(7, "file not in format");
    if(sizeof(compH_t) + sizeof(compEl_t) * compH.count != (long unsigned int)s2.st_size) errx(10, "invalid count");

    compEl_t compEl;
    for(uint64_t i = 0; i < compH.count; i++) {
        if(read(fd2, &compEl, sizeof(compEl)) != sizeof(compEl)) err(11, "cant read");
        if(compEl.type != 0 && compEl.type != 1) errx(12, "wrong format");
        uint64_t byteOffset1 = sizeof(dataH_t) + compEl.offset1 * sizeof(uint64_t);
        uint64_t byteOffset2 = sizeof(dataH_t) + compEl.offset2 * sizeof(uint64_t);

        if(byteOffset1 > (uint64_t)s1.st_size || byteOffset2 > (uint64_t)s2.st_size ) errx(13, " too big offset");
        uint64_t first = readFrom(fd1, byteOffset1);
        uint64_t second = readFrom(fd1, byteOffset2);

        if(shouldSwap(first, second, compEl.type))
            mySwap(fd1, byteOffset1, byteOffset2, first, second);
    }

    close(fd1);
    close(fd2);
    return 0;
}
