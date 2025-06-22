#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>

bool isBitSet(uint8_t byte, uint8_t pos);
bool isBitSet(uint8_t byte, uint8_t pos) {
    return (1 << (7 - pos)) & byte;
}

int main(int argc, char* argv[]) {

    if(argc != 4) errx(1, "3 params needed");

    int fd1 = open(argv[1], O_RDONLY);
    if(fd1 == -1) err(2, "Cant open file");

    int fd2 = open(argv[2], O_RDONLY);
    if(fd2 == -1) err(2, "Cant open file");

    int fd3 = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(fd3 == -1) err(2, "Cant open file");

    struct stat s1, s2;
    if(fstat(fd1, &s1) == -1) err(3, "Cant stat");
    if(fstat(fd2, &s2) == -1) err(3, "Cant stat");
    uint32_t size1 = s1.st_size, size2 = s2.st_size;
    if(size2 % sizeof(uint16_t) != 0) errx(5, "File broken");

    if(size1 * 8 != size2 / sizeof(uint16_t)) errx(4, "Files dont match sizes");

    uint8_t curBit = 0;
    int8_t rd = 0;
    uint8_t curByte = 0;
    uint16_t el;
    while(true){
        if(curBit % 8 == 0) {
            curBit = 0;
            rd = read(fd1, &curByte, sizeof(curByte));
            if(rd  == 0) break;
            else if(rd == -1) err(6, "Cant read");
        }


        if(read(fd2, &el, sizeof(el)) != sizeof(el)) err(7, "Cant read");
        if(isBitSet(curByte,curBit)) {
            if(write(fd3, &el, sizeof(el)) != sizeof(el)) err(8, "Cant write");
        }
        curBit++;
    }

    close(fd1);
    close(fd2);
    close(fd3);
    return 0;
}
