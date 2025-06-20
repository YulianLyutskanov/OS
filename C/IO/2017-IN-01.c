#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>

struct triple {
    uint16_t offset;
    uint8_t len;
    uint8_t saved;
};

typedef struct triple triple_t;

int main(int argc, char* argv[]) {

    if(argc != 5) errx(1, "4 Params needed");

    int fd1 = open(argv[1], O_RDONLY);
    if(fd1 == -1) err(2, "Can't open file 1");

    int fd2 = open(argv[2], O_RDONLY);
    if(fd2 == -1) err(3, "Can't open file 2");

    struct stat st1;
    if(fstat(fd1, &st1) == -1) err(6, "Can't get stat of 1st file");

    struct stat st2;
    if(fstat(fd2, &st2) == -1) err(6, "Can't get stat of 2nd file");

    if(st2.st_size % sizeof(triple_t) != 0) errx(7, "2nd file is not in the needed format");

    int fd3 = open(argv[3], O_TRUNC | O_CREAT | O_WRONLY, 0666);
    if(fd3 == -1) err(4, "Can't open file 3 for writing");

    int fd4 = open(argv[4], O_TRUNC | O_CREAT | O_WRONLY, 0666);
    if(fd4 == -1) err(5, "Can't open file 4 for writing");

    int16_t readBytes;
    triple_t t;
    uint32_t curPos = 0;
    uint8_t buff[256];

    while((readBytes = read(fd2, &t, sizeof(t))) == sizeof(t)){

        if(t.offset >= st1.st_size) errx(10, "Invalid triplet format");

        if(lseek(fd1, t.offset, SEEK_SET) < 0 ) err(8, "Can't seek");

        if(read(fd1, &buff, t.len) != t.len) err(9, "Can't read");

        if(buff[0] >= 'A' && buff[0] <= 'Z') {
            if(write(fd3, &buff, t.len) != t.len) err(9, "Can't write");
            triple_t new = {curPos, t.len, 0};
            if(write(fd4, &new, sizeof(new)) !=  sizeof(new) ) err(9, "Can't write");
            curPos += t.len;
        }

    }

    if(readBytes < 0 ) err(8, "Can't read from file 2");

    close(fd1);
    close(fd2);
    close(fd3);
    close(fd4);
    return 0;
}
