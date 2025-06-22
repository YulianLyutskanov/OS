#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

struct head {
    uint32_t magic;
    uint8_t headerVersion, dataVersion;
    uint16_t count;
    uint32_t reserved1, reserved2;
};

struct v0 {
    uint16_t offset;
    uint8_t old, new;
};

struct v1 {
    uint32_t offset;
    uint16_t old, new;
};

typedef struct head head_t;
typedef struct v0 v0_t;
typedef struct v1 v1_t;

void copyFile(int from, int to);
void copyFile(int from, int to) {
    uint8_t byte;
    int8_t bytesRead;
    while((bytesRead = read(from, &byte, sizeof(byte))) == sizeof(byte)) {
        if(write(to, &byte, sizeof(byte)) != sizeof(byte)) err(14, "Cant write");
    }
    if(bytesRead == -1) err(15, "Cant read");
}

int main(int argc, char* argv[]) {

    if(argc != 4) errx(1, "4 params needed");

    int fdh = open(argv[1], O_RDONLY);
    if(fdh == -1) err(2, "Cant open file");

    struct stat sh;
    if(fstat(fdh, &sh) == -1) err(3, "Cant stat");
    uint32_t hSize = sh.st_size;
    if(hSize < sizeof(head_t)) errx(4, "invalid format");

    head_t h;
    if((read(fdh, &h, sizeof(h))) != sizeof(h)) err(5, "Cant read");

    if(h.magic != 0xEFBEADDE) errx(6, "Wrong magic");
    if(h.headerVersion != 0x01) errx(7, "Wrong header version");

    if(h.dataVersion != 0x00 && h.dataVersion != 0x01) errx(8, "Wrong data version");

    int fd1 = open(argv[2], O_RDONLY);
    if(fd1 == -1) err(9, "Cant open file");
    struct stat s1;
    if(fstat(fd1, &s1) == -1) err(3, "Cant stat");

    int fd2 =  open(argv[3], O_TRUNC | O_CREAT | O_RDWR, 0666);
    if(fd2 == -1) err(10, "Cant open file");
    copyFile(fd1,fd2);

    if(h.dataVersion == 0x00) {
        if((hSize - sizeof(head_t)) != sizeof(v0_t) * h.count) errx(11, "Invalid bin format");
        if(s1.st_size > UINT16_MAX) errx(12, "File too big");

        v0_t data;
        uint8_t byte;
        for(uint32_t i = 0; i < h.count; i++) {
            if(read(fdh, &data, sizeof(data)) != sizeof(data)) err(16, "Cant read");
            if(data.offset > s1.st_size) errx(17, "Invalid offset");
            if(lseek(fd2, data.offset, SEEK_SET) == -1) err(18, "Cant seek");
            if(read(fd2, &byte, sizeof(byte)) != sizeof(byte)) err(19, "Cant read");
            if(byte != data.old) errx(20, "Invalid original byte");
            if(lseek(fd2, data.offset, SEEK_SET) == -1) err(18, "Cant seek");
            if(write(fd2, &data.new, sizeof(byte)) != sizeof(byte)) err(21, "Cant write");

        }

    } else if(h.dataVersion == 0x01) {
        if((hSize - sizeof(head_t)) != sizeof(v1_t) *h.count) errx(12, "Invalid bin format");
        if(s1.st_size > UINT32_MAX) errx(12, "File too big");

        v1_t data;
        uint16_t byte;
        for(uint64_t i = 0; i < h.count; i++) {
            if(read(fdh, &data, sizeof(data)) != sizeof(data)) err(16, "Cant read");
            if(data.offset > s1.st_size) errx(17, "Invalid offset");
            if(lseek(fd2, data.offset, SEEK_SET) == -1) err(18, "Cant seek");
            if(read(fd2, &byte, sizeof(byte)) != sizeof(byte)) err(19, "Cant read");
            if(byte != data.old) errx(20, "Invalid original byte");
            if(lseek(fd2, data.offset, SEEK_SET) == -1) err(18, "Cant seek");
            if(write(fd2, &data.new, sizeof(byte)) != sizeof(byte)) err(21, "Cant write");

        }

    }


    close(fdh);
    close(fd1);
    close(fd2);

    return 0;
}
