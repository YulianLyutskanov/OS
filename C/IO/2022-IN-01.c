#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>

struct head {
    uint16_t magic;
    uint16_t filetype;
    uint32_t count;
};

struct pair {
    uint16_t pos, val;
};

typedef struct head head_t;
typedef struct pair pair_t;

void mySeek(int fd, uint32_t pos);
void mySeek(int fd, uint32_t pos) {
    if(lseek(fd, pos, SEEK_SET) == -1) err(9, "Cant seek");
}

int main(int argc, char* argv[]) {

    if(argc != 4) errx(1, "3 params needed");

    int fd1 = open(argv[1], O_RDONLY);
    if(fd1 == -1) err(2, "Cant open file");

    int fd2 = open(argv[2], O_RDONLY);
    if(fd2 == -1) err(2, "Cant open file");

    int fd3 = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(fd3 == -1) err(2, "Cant open file");

    struct stat s;
    if(fstat(fd1, &s) == -1) err(3, "Cant stat");
    uint32_t size1 = s.st_size;
    if(size1 < sizeof(head_t)) errx(4, "File broken");
    if(fstat(fd2, &s) == -1) err(3, "Cant stat");
    uint32_t size2 = s.st_size;
    if(size2 < sizeof(head_t)) errx(4, "File broken");

    head_t h1, h2;
    if(read(fd1, &h1, sizeof(h1)) != sizeof(h1)) err(5, "Can't read");
    if(read(fd2, &h2, sizeof(h2)) != sizeof(h2)) err(5, "Can't read");
    if(h1.magic != 0x5A4D || h2.magic != 0x5A4D) errx(9, "not in Hoge");
    if(h1.filetype != 1 || h2.filetype != 2) errx(10, "Not right type");

    if(size1 != sizeof(head_t) + h1.count * sizeof(uint16_t)) errx(6, "Invalid format");
    if(size2 != sizeof(head_t) + h2.count * sizeof(uint32_t)) errx(6, "Invalid format");
    if(h2.count % 2 != 0) errx(7,"Broken file");

    uint32_t pairsCount = h2.count / 2;
    pair_t p;
    uint32_t el;

    for(uint32_t i = 0; i < pairsCount; i++) {
        if(read(fd1, &p, sizeof(p)) != sizeof(p)) err(8, "Cant read");
        if(sizeof(head_t) + p.pos * sizeof(uint32_t) > size2) errx(13, "File not that long");
        printf("read from %d \n",(int)(sizeof(head_t) + p.pos * sizeof(uint32_t)));
        mySeek(fd2, sizeof(head_t) + p.pos * sizeof(uint32_t));
        if(read(fd2, &el, sizeof(el)) != sizeof(el)) err(11, "Cant read");
        mySeek(fd3, sizeof(head_t) + p.val * sizeof(uint64_t));
        if(write(fd3, &el, sizeof(el)) != sizeof(el)) err(12, "Cant write");
    }

    close(fd1);
    close(fd2);
    close(fd3);
    return 0;
}
