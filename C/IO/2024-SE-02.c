#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

struct head {
    uint32_t magic, count;
    uint64_t originalSize;
};

typedef struct head head_t;

int main(int argc, char* argv[]) {

    if(argc != 3) errx(1, "3 args needed");

    int fd1 = open(argv[1], O_RDONLY);
    if(fd1 == -1) err(2, "cant open");

    int fd2 = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(fd2 == -1) err(2, "cant open");

    struct stat s;
    if(fstat(fd1, &s) == -1) err(3, "cant stat");
    if(s.st_size < (long int)sizeof(head_t)) errx(4, "file too small");

    head_t head;
    if(read(fd1, &head, sizeof(head)) != sizeof(head)) err(5, "cant read");

    uint8_t byte;
    int8_t bytesRead;
    for(uint32_t i = 0; i < head.count; i++) {
        if((bytesRead = read(fd1, &byte, sizeof(byte))) == 0) errx(6, "file invalid");
        if(bytesRead == -1) err(7, "cant read");

        uint8_t type = byte & (1 << 7);
        uint8_t n= byte & ((1 << 7) - 1);
        if(type == 0) {
            for(uint8_t j = 0; j < n + 1; j++) {
                if((bytesRead = read(fd1, &byte, sizeof(byte))) == 0) errx(6, "file invalid");
                if(bytesRead == -1) err(7, "cant read");
                if(write(fd2, &byte, sizeof(byte)) != sizeof(byte)) err(8, "cant write");
            }
        } else {
            if((bytesRead = read(fd1, &byte, sizeof(byte))) == 0) errx(6, "file invalid");
            if(bytesRead == -1) err(7, "cant read");
            for(uint8_t j = 0; j < n + 1; j++) {
               if(write(fd2, &byte, sizeof(byte)) != sizeof(byte)) err(8, "cant write");
            }
        }
    }

    close(fd1);
    close(fd2);
    return 0;
}
