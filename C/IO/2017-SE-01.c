#include <stdint.h>
#include <err.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

struct triple{
    uint16_t offset;
    uint8_t old;
    uint8_t new;
};

typedef struct triple triple_t;

int main(int argc, char* argv[]) {

    if(argc != 4) errx(1, "3 params needed");

    int fd1 = open(argv[1], O_RDONLY);
    if(fd1 == -1) err(2, "Can't open file");

    int fd2 = open(argv[2], O_RDONLY);
    if(fd2 == -1) err(3, "Can't open file");

    struct stat s1, s2;
    if(fstat(fd1, &s1) == -1) err(4, "Cant stat");
    if(fstat(fd2, &s2) == -1) err(5, "Cant stat");

    if(s1.st_size != s2.st_size) errx(6, "Files must have same size");

    int fd3 = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(fd3 == -1) err(7, "Can't open file to write");

    uint32_t curOffset = 0;
    uint8_t first, second;
    int8_t bytesReadF, bytesReadS;

    while((bytesReadF = read(fd1, &first, sizeof(first))) == sizeof(first) &&
          (bytesReadS = read(fd2, &second, sizeof(second))) == sizeof(second)) {

        if(first != second) {
            triple_t t = {curOffset, first, second};
            if(write(fd3, &t, sizeof(t)) != sizeof(t)) err(9, "Cant write to file");
        }
        curOffset++;
    }

    if(bytesReadF < 0 || bytesReadS < 0) err(8, "Cant read from files");
    close(fd1);
    close(fd2);
    close(fd3);
    return 0;
}
