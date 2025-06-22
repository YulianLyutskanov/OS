#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>


bool isBitSet(uint8_t byte, uint8_t pos);
bool isBitSet(uint8_t byte, uint8_t pos) {
    return (1 << pos) & byte;
}
uint8_t encodeRightHalf(uint8_t byte) {
    uint8_t res = 0;

    for(uint8_t i = 0; i < 4; i++) {
        if(isBitSet(byte, i))
            res |= (1 << (2 * i + 1));
        else
            res |= (1 << (2 * i));
    }

    return res;
}

int main(int argc, char* argv[]) {

    if(argc != 3) errx(1, "2 params needed");

    int fd1 = open(argv[1], O_RDONLY);
    if(fd1 == -1) err(2, "Cant open file");

    int fd2 = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(fd2 == -1) err(2, "Cant open file");

    uint8_t byte;
    int8_t bytesRead;

    while((bytesRead = read(fd1, &byte, sizeof(byte))) == sizeof(byte)) {
        uint8_t right = encodeRightHalf(byte);
        uint8_t left = encodeRightHalf(byte >> 4);
        if(write(fd2, &left, sizeof(left)) != sizeof(left)) err(5, "Cant write");
        if(write(fd2, &right, sizeof(right)) != sizeof(right)) err(5, "Cant write");

    }

    if(bytesRead == -1) err(4, "Cant read");
    close(fd1);
    close(fd2);

    return 0;
}
