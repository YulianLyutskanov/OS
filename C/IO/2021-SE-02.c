#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>

bool isBitSet(uint8_t byte, uint8_t pos);
bool isBitSet(uint8_t byte, uint8_t pos) {
    return (1 << pos) & byte;
}

uint8_t decode(uint8_t* b);
uint8_t decode(uint8_t* b) {
    uint8_t res = 0;

    for(uint8_t i = 0; i < 4; i++) {
        if(isBitSet(b[1], 2 * i) == isBitSet(b[1], 2 * i + 1)) errx(7, "Wrong format");
        if(!isBitSet(b[1], 2 * i) && isBitSet(b[1], 2 * i + 1)) {
            res |= (1 << i);
        }

        if(isBitSet(b[0], 2 * i) == isBitSet(b[0], 2 * i + 1)) errx(7, "Wrong format");
        if(!isBitSet(b[0], 2 * i) && isBitSet(b[0], 2 * i + 1)) {
            res |= (1 << (i + 4));
        }
      }
    return res;
}

int main(int argc, char* argv[]) {

    if(argc != 3) errx(1, "2 params needed");

    int fd1 = open(argv[1], O_RDONLY);
    if(fd1 == -1) err(2, "Cant open file");

    int fd2 = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(fd2 == -1) err(2, "Cant open file");

    struct stat s;
    if(fstat(fd1, &s) == -1) err(3, "Cant stat");
    uint64_t size = s.st_size;
    if(size % 2 != 0) errx(4, "Broken file");

    uint8_t b[2];
    int8_t bytesRead;
    uint64_t outputSize = size / 2;

    for(uint64_t i = 0; i < outputSize; i++) {
        bytesRead = read(fd1, &b, sizeof(b));
        if(bytesRead != sizeof(b)) err(5, "Cant read");
        uint8_t decoded = decode(b);
        if(write(fd2, &decoded, sizeof(decoded)) != sizeof(decoded)) err(8, "Cant write");
    }

    close(fd1);
    close(fd2);

    return 0;

}
