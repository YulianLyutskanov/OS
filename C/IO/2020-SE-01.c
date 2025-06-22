#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

struct head {
    uint32_t u1;
    uint16_t count, u2;
    uint32_t u3, u4;
};

struct interval {
    uint16_t start, len;
};

typedef struct interval interval_t;

struct set {
    interval_t intervals[4];
};

typedef struct head head_t;
typedef struct set set_t;

uint16_t checkCount(int fd, uint8_t sizeEl);
uint16_t checkCount(int fd, uint8_t sizeEl) {
    head_t h;
    if(read(fd, &h, sizeof(h)) != sizeof(h)) err(4,"Cant read");
    struct stat s;
    if(fstat(fd, &s) == -1) err(5, "Cant stat");
    if(s.st_size != (long int)sizeof(h) + sizeEl * h.count) err(6, "File size is wrong");
    return h.count;
}

uint8_t el1;
uint16_t el2;
uint32_t el4;
uint64_t el8;

void myWrite(uint16_t from, uint16_t to, interval_t interval, uint8_t sizeEl);
void myWrite(uint16_t from, uint16_t to, interval_t interval, uint8_t sizeEl) {
    void* el = NULL;
    if(sizeEl == 1) el = &el1;
    else if (sizeEl == 2) el = &el2;
    else if (sizeEl == 4) el = &el4;
    else if (sizeEl == 8) el = &el8;

    if(lseek(from, interval.start * sizeEl + sizeof(head_t), SEEK_SET) == - 1) err(9, "cant seek");

    for(uint16_t i = 0; i < interval.len; i++){
       if(read(from, el, sizeEl) != sizeEl) err(10, "cant read");
       if(write(to, el, sizeEl) != sizeEl) err(11, "Cant write");
    }
}

int main(int argc, char* argv[]) {

    if(argc != 7) errx(1, "6 params needed");


    int fd1 = open(argv[1], O_RDONLY);
    if(fd1 == -1) err(2, "Can't open file");

    int fd2 = open(argv[2], O_RDONLY);
    if(fd2 == -1) err(2, "Can't open file");

    int fd3 = open(argv[3], O_RDONLY);
    if(fd3 == -1) err(2, "Can't open file");

    int fd4 = open(argv[4], O_RDONLY);
    if(fd4 == -1) err(2, "Can't open file");

    int fd5 = open(argv[5], O_RDONLY);
    if(fd5 == -1) err(2, "Can't open file");

    int fd6 = open(argv[6], O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(fd6 == -1) err(2, "Can't open file");

    uint16_t elCounts[6];
    elCounts[1] = checkCount(fd1, 2);
    elCounts[2] = checkCount(fd2, 4);
    elCounts[3] = checkCount(fd3, 1);
    elCounts[4] = checkCount(fd4, 2);
    elCounts[5] = checkCount(fd5, 8);
    uint16_t numOfSets = elCounts[1] / (4 * 2);

    set_t s;
    for(uint16_t i = 0; i < numOfSets; i++) {
        if(read(fd1, &s, sizeof(s)) != sizeof(s)) err(6, "Cant read");

        if(s.intervals[0].start + s.intervals[0].len - 1 > elCounts[2]) errx(7, "invalid interval");
        myWrite(fd2, fd6, s.intervals[0], 4);

        if(s.intervals[1].start + s.intervals[1].len - 1 > elCounts[3]) errx(8, "invalid interval");
        myWrite(fd3, fd6, s.intervals[1], 1);

        if(s.intervals[2].start + s.intervals[2].len - 1 > elCounts[4]) errx(9, "invalid interval");
        myWrite(fd4, fd6, s.intervals[2], 2);

        if(s.intervals[3].start + s.intervals[3].len - 1 > elCounts[5]) errx(10, "invalid interval");
        myWrite(fd5, fd6, s.intervals[3], 8);

    }

    close(fd1);
    close(fd2);
    close(fd3);
    close(fd4);
    close(fd5);
    close(fd6);
    return 0;
}
